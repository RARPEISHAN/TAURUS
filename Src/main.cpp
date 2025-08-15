#include "Taurus.h"
#include "./Core/Reactor.h"
#include "./Core/StreamServer.h"
#include "./Core/NetAddress.h"
#include "./Core/ReactorFactory.h"
#include "./Core/StreamClient.h"
#include <stdio.h>
#include <fstream>
#include<iostream>
constexpr char file[] = R"(E:\编程\窗口程序设计\01-04-2021-a.mp4)";/*  R"(C:\Users\ASUS\Desktop\何明\C++面试题集锦(1).docx)"  */
#define BUFFERSIZE 64*1024
class FileHeader
{
public:
	wchar_t _fileName[MAX_PATH];
	unsigned long long int _fileSize;
};
using namespace std;
int alMain()
{
	using aldebaran::core::BUFSIZE;
	auto reactorFactory = aldebaran::make_obj<aldebaran::core::ReactorFactory>();

	auto reactor = reactorFactory->createReactor();

	auto netaddress = aldebaran::make_obj<aldebaran::core::NetAddress>(L"127.0.0.1", (unsigned short)4001);
	auto streamServe = aldebaran::make_obj<aldebaran::core::StreamServer>(reactor, netaddress);

	streamServe->start();
	while (true)
	{
		auto connection = streamServe->accept().get();
		if (connection)
		{
			std::thread t([&]() {
				FileHeader fileHeader;

				fstream fs;
				fs.open(file, fstream::binary | fstream::in);
				if (!fs)
					return;

				/* move to last pos. */
				fs.seekg(0, fstream::end);
				fileHeader._fileSize = fs.tellg();
				wcscpy(fileHeader._fileName, (wchar_t*)file);
				fs.seekg(0, fstream::beg);

				if (!connection->expired())
				{
					connection->lock()->send(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader))->get();
				}

				using namespace std;
				vector<char> buf(BUFSIZE);
				unsigned long long int remainder = fileHeader._fileSize;

				/* move to last pos. */

				while (remainder > 0)
				{
					fs.read(buf.data(), BUFSIZE);
					remainder -= fs.gcount();
					/* send the buffer to client. */
					if (!connection->expired())
					{
						auto i = connection->lock()->send(reinterpret_cast<const char*>(buf.data()), fs.gcount())->get();
						i < 0 ? i = 0 : i;
						while (i < fs.gcount())
						{
							auto it = connection->lock()->send(reinterpret_cast<const char*>(buf.data() + i), fs.gcount() - i)->get();
							i += (it < 0 ? 0 : it);
						}
					}
				}
				});
			t.detach();
		}
	}

	reactor->quit();

	return aldebaran::core::defOver();
}