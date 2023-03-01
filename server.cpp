#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void write_error(const char *s)
{
    std::cerr << s << std::endl;
    exit (1);
}

void    response_client(char *s, int client_socket)
{
    ssize_t bytes_sent = send(client_socket, s, std::strlen(s), 0);
    if (bytes_sent < 0)
        write_error("Error sending");
    else
        std::cout << s; //en verdad no cunde imprimir esto
}

int main() 
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); //socket(int domain, int type, int protocol)
    if (server_socket < 0)
        write_error("Error opening");

    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
        write_error("Error binding");

    if (listen(server_socket, 5) < 0)
        write_error("Error listening");

    std::cout << "Listening on port " << "8080..." << std::endl; //poner var en un futuro y no escribir a mano

    while (1)
    {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0)
        {
            std::cerr << "Error accepting client connection" << std::endl;;
            continue; //saltamos el resto de iteracion del bucle y volvemos a entrar
        }

        std::cout << "Accepted client connection" << std::endl;

        while (1)
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
            //response_client("Hello, client!", client_socket); //funcion para devolver el mensaje al cliente. La respuesta se hace cada vez que el cliente mande algo.
        }
    }
}

