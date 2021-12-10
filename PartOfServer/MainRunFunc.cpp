#include <iostream>
#include <WS2tcpip.h> // сокеты windows API для доступа к сетевым сокетам, будем работать через них. К тому 
// же он включает ф-ю winsock и много других фу-й, которые мы будем юзать в ходе разработки
#pragma comment(lib, "ws2_32.lib")

using namespace std; // да-да знаю, bad practice, но учитывая что это дом. работа, упустим излишества


void main() 
{
	setlocale(LC_ALL, "Russian");
	// Инициализируем винсокет
	// Создаем сокет и биндим (коннектим) его 
	// Биндим сокет с полученным IP адресом  и портом
	// Указываем винсокету, что мы открыли совет в ожидании прослушивания
	// И ждем, пока подключимся
	// Закрываем прослушивание сокета 
	// LOOP: получаем доступ и выводим тематическое смс клиенту

	WSAData WinSockObj;  // структура для создания winsocks
	WORD version = MAKEWORD(2, 2); // версия winsocks, которые мы будем использовать
	int winSock = WSAStartup(version, &WinSockObj);  // передаем в ф-ю версию нашего винсокса и адрес самого obj winsocks
	if (winSock != 0) 
	{
		cerr << "Программа не может создать winsock!" << endl; // думаю по циклу ясно, а поток выбрали конечно соответственный для ошибок
		return;
	}

	SOCKET listenSockServer = socket(AF_INET, SOCK_STREAM, 0); // версия inet  - IPV4 , используем поток сокет, а последнее значение просто ноль.
	if (listenSockServer == INVALID_SOCKET)  // +- тоже самое, что в предыдущем цикле, только отличает второй операнд на сравнение
		{
		cerr << "Программа не может создать сокет!" << endl;
		return;
		}


	sockaddr_in addressIPForSocket; // создаем на основе структуры объект на основе выбранного протокола
	int serverSizeAddr = sizeof(addressIPForSocket);
	addressIPForSocket.sin_family = AF_INET; // семейство inet - IPV4
	addressIPForSocket.sin_port = htons(40000); // способ передачи байтов от хоста к сети, номер порта рандом - главное, чтобы он у вас бы не занят какой то службой
	addressIPForSocket.sin_addr.S_un.S_addr = INADDR_ANY; 


	bind(listenSockServer, (sockaddr*)&addressIPForSocket, serverSizeAddr); // связываем наш сокет с портом, указатель на адрес и размер структуры адрес
	listen(listenSockServer, SOMAXCONN); // начинаем чекать сокет на прослушку порта, SOMAXCONN максимальная длина очереди, которая прослушивает порт.
	

	sockaddr_in addrClient; 				// начинаем ожидать первых подключений 
	int clientSizeAddr = sizeof(addrClient); // получаем размер адреса клиента
	SOCKET clientSocket = accept(listenSockServer, (sockaddr*)&addrClient, &clientSizeAddr); // слушаем, получаем адрес, порт по которому клиент может приконектиться
	if (clientSocket == INVALID_SOCKET) 
	{
		cerr << "Программе не удалось подключить клиента по данному порту!" << endl;
	}


	char host[NI_MAXHOST]; // для получение имени клиента, который приконектился
	char service[NI_MAXHOST]; // порт по которому клиент приконектился
	ZeroMemory(host, NI_MAXHOST); // default заполняет память нулями, первый параметр это указатель на блок памяти, который нужно заполнить, а второй размер в байтах этой же памяти
	ZeroMemory(service, NI_MAXSERV);
	// getnameinfo это ф-ю которая преобра. адрес сокета в независимый хост\службу, притом похерестично на каком протоколе это базируется.
	if (getnameinfo((sockaddr*)&clientSocket, clientSizeAddr, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) // одним словом, если коннект удался...
	{
		cout << host << "  Удачно подключился по порту " << service << endl;
	}
	else 
	{
		inet_ntop(AF_INET, &addrClient.sin_addr, host, NI_MAXHOST);
		cout << host << " подключение по порту " << htons(addrClient.sin_port) << endl;
	}
	closesocket(listenSockServer);

	char bufferSize[10000]; // размер буфера, делайте какой захотите сами
	while (true) // ожидаем клиента, пока он не отправит какие либо данные
	{
		ZeroMemory(bufferSize, 10000);
		int dataReceived = recv(clientSocket, bufferSize, 10000, 0); // ф-ю для отправки данных
		if (dataReceived == SOCKET_ERROR) 
		{
			cerr << "Программа словила ошибку в отправке данных" << endl;
			break;
		}
		if (dataReceived == 0) // если ничего не получили, значить клиент вышел
		{
			cout << "Клиент покинул чат" << endl;
			break;
		}
		cout << string(bufferSize, 0, dataReceived) << endl;
		send(clientSocket, bufferSize, dataReceived + 1, 0); // сообщение для отправки клиенту, размер n+1 для нулевого символа
	}
	closesocket(clientSocket);
	WSACleanup(); // закрываем нашу лавочку и очищаем winsock
	// Закрываем сокет
	// Выключаем сокет


}