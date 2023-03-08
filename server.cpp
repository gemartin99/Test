#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/poll.h>
#include <vector>

#define MAX_CLIENTS 128

static bool is_digits(const std::string &str)
{
    return (str.find_first_not_of("0123456789") == std::string::npos);
}

void write_error(const char *s)
{
    std::cerr << s << std::endl;
    exit (EXIT_FAILURE);
}

int main(int argc, char **argv) 
{
    if (argc != 3)
        write_error("Invalid arguments");
    int port = atoi(argv[1]);
    if (!is_digits(argv[1]) || port > 65535 || port < 1)
        write_error("Invalid port");
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        write_error("Error creating socket");

    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
        write_error("Error binding"); //error al asociar el socket a la direccion del servidor

    if (listen(server_socket, MAX_CLIENTS) == -1)
        write_error("Error listening"); //error al escuchar conexiones entrantes

    // declaracion estructura de eventos para la funcion poll
    struct pollfd fd_array[MAX_CLIENTS + 1];

    // configuracion del evento para el socket del servidor
    fd_array[0].fd = server_socket;
    fd_array[0].events = POLLIN;

    int num_fds = 1;

    std::vector<int> vec_clients;
    std::cout << "Listening on port " << port << std::endl;
    // bucle a la espera de conexiones entrantes y manejarlas
    while (true)
    {
        int num_events = poll(fd_array, num_fds, -1);

        if (num_events == -1)
            write_error("Error poll");

        for (int i = 0; i < num_fds; i++)
        {
            if (fd_array[i].revents == 0)
                continue;

            // check nueva conexion
            if (fd_array[i].fd == server_socket)
            {
                int client_fd = accept(server_socket, nullptr, nullptr);
                if (client_fd == -1)
                {
                    std::cerr << "Error accepting incoming connection" << std::endl;
                    continue;
                }
                std::cout << client_fd << MAX_CLIENTS << std::endl;
                if (num_fds == MAX_CLIENTS)
                {
                    std::string welcome_msg = "The server is full. Please try again later.\n";
                    send(client_fd, welcome_msg.c_str(), welcome_msg.length(), 0);
                    close(client_fd);
                    continue;
                }
                vec_clients.push_back(client_fd);
                // configurar el evento para el nuevo cliente
                fd_array[num_fds].fd = client_fd;
                fd_array[num_fds].events = POLLIN;
                num_fds++;

                std::string welcome_msg = "Bienvenido al servidor IRC\n";
                send(client_fd, welcome_msg.c_str(), welcome_msg.length(), 0);

                std::cout << "New client FD = " << client_fd << std::endl;
            }
            else 
            {
                char buffer[512];
                memset(buffer, 0, sizeof(buffer));
                int bytes_received = recv(fd_array[i].fd, buffer, sizeof(buffer), 0);

                if (bytes_received == -1) 
                {
                    std::cerr << "Error receiving customer data FD = " << fd_array[i].fd << std::endl;
                    continue;
                }

                if (bytes_received == 0) //si un cliente se desconecta se cierra su socket, se elimina del vector y se elimina el evento de la estructura
                {
                    vec_clients.erase(std::remove(vec_clients.begin(), vec_clients.end(), fd_array[i].fd), vec_clients.end());
                    std::cout << "Client disconnected FD = " << fd_array[i].fd << std::endl;
                    close(fd_array[i].fd);
                    for (int j = i; j < num_fds; j++)
                    {
                        fd_array[j].fd = fd_array[j + 1].fd;
                        fd_array[j].events = fd_array[j + 1].events;
                    }
                    num_fds--;
                }
                else
                {
                    std::cout << buffer;
                    //send(fd_array[i].fd, buffer, bytes_received, 0);
                }
            }
        }
    }
    close(server_socket);
    return 0;
}