#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static bool is_digits(const std::string &str)
{
    return (str.find_first_not_of("0123456789") == std::string::npos);
}

void write_error(const char *s)
{
    std::cerr << s << std::endl;
    exit (1);
}

void    response_client(const char *s, int client_socket)
{
    ssize_t bytes_sent = send(client_socket, s, std::strlen(s), 0);
    if (bytes_sent < 0)
        write_error("Error sending");
    else
        std::cout << s; //en verdad no cunde imprimir esto
}

int main(int argc, char **argv) 
{
    if (argc != 3)
        write_error("Invalid arguments");
    int port = atoi(argv[1]);
    if (!is_digits(argv[1]) || port > 65535 || port < 1)
        write_error("Invalid port");
    /*if (strlen(argv[2]) < 7) //hacer parsing de contraseña en el futuro
        write_error("Insecure password");*/
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); //socket(int domain, int type, int protocol)
    if (server_socket < 0)
        write_error("Error opening");

    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET; //para direcciones lPv4.
    //server_address.sin_addr.s_addr = htonl(INADDR_ANY); //direccion ip de la direccion del socket en formato de bytes de red
    server_address.sin_addr.s_addr = inet_addr("10.11.8.4");
    server_address.sin_port = htons(port); //puerto de la direccion del socket en formato bytes de red

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) //asocia el numero de puerto y la IP con el socket.
        write_error("Error binding");

    if (listen(server_socket, 5) < 0) //modificar el 5 a las conexiones permitidas que nos interese
        write_error("Error listening");

    std::cout << "Listening on port " << port << std::endl; //poner var en un futuro y no escribir a mano el puerto

    while (1) //se queda en escucha a la espera de que el cliente se conecte
    {
        //⚠️ IMPORTANTE HACER ARRAY DE CLIENT SOCKETS ⚠️
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0)
        {
            std::cerr << "Error accepting client connection" << std::endl;;
            continue; //saltamos el resto de iteracion del bucle y volvemos a entrar
        }

        std::cout << "Accepted client connection" << std::endl;

        while (1) //se queda en escucha infinita a la espera de nuevos mensajes. MODIFICAR PARA EL FUTURO que coja mensajes hasta que le llegue \r\n y cuando le llegue pasar todos los mensajes al parsing
        {
            char buffer[1024];
            std::memset(buffer, 0, sizeof(buffer));
            ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received < 0)
            {
                std::cerr << "Error receiving data from client" << std::endl;
                break;
            }
            else if (bytes_received == 0)
            {
                std::cout << "Client disconnected" << std::endl;
                break;
            }
            else
            {
                std::cout << buffer;// << std::endl;
                //std::cout << "Received " << bytes_received << " bytes: " << buffer << std::endl;
            }
            const char *responde_msg = "Mensaje recibido caranalga!"; //"Hello, client!";
            response_client(responde_msg, client_socket); //funcion para devolver el mensaje al cliente. La respuesta se hace cada vez que el cliente mande algo.
        }
    }
}

