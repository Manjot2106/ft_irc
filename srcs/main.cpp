# include "IRC.hpp" 

int	main(int ac, char **av)
{
	int port;

	if (ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return (1);
	}
	if (!isValidPort(av[1], port))
	{
		std::cerr << "Error: invalid port" << std::endl;
		return (1);
	}
	try
	{
		Server server(port, av[2]);
		server.initialize();
		std::cout << " IRC server listening on port " << port << std::endl;
		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: "<< e.what() << '\n';
		return (1);
	}
	return (0);
}