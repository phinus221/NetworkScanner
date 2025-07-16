#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <bits/stdc++.h>
using namespace std;

int scan_ports(const char* ip_addr)
{
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;

  // COnverting IPv4 from string to binary
  const char* const_ip_addr = (const char*)ip_addr;
  cout << "Converting the ip address" << endl;

  if (inet_pton(AF_INET, const_ip_addr, &serv_addr.sin_addr) <= 0) 
  {
    cerr << "Invalid address/ Address not supported" << endl;
    return -1;
  }

  for(int i = 0; i <= 65535; i++)
  {
    //Dictionary for the most common ports
    map<int, string> port_services = {
     {21, "FTP"}, {22, "SSH"}, {23, "Telnet"},
     {25, "SMTP"}, {53, "DNS"}, {80, "HTTP"},
     {110, "POP3"}, {143, "IMAP"}, {443, "HTTPS"},
     {3306, "MySQL"}, {8080, "HTTP (Alt)"}}; 

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
      cerr << "Socket creation error" << endl;
      return -1;
    }

    serv_addr.sin_port = htons(i);
    //cout << "Trying port " << i << endl;

    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0)
    {
      cout << "Port " << i << " is open." << endl;
      
      for(auto& port_number : port_services)
      {
        if(i == port_number.first)
          cout << port_number.second << endl;
      }

      if(i == 80 || i == 8080)
      {
        const char* http_req = "GET / HTTP/1.0\r\n\r\n";
        send(sock, http_req, strlen(http_req), 0);
      }
      
      // Setting a timeout so we dont hang on recv()
      struct timeval timeout;
      timeout.tv_sec = 5;
      timeout.tv_usec = 0;
      setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

      char buffer[100] = {0};
      int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
      if(bytes > 0)
      {
        buffer[bytes] = '\0';
        cout << buffer << endl;
      }
      else {
        cout << "No information on said port" << endl;
      }
    }
    close(sock);
  }

  return 0;
}

int main()
{
  string ip_addr;
  cout << "Enter the IP address to scan: ";
  cin >> ip_addr;
  
  scan_ports(ip_addr.c_str());

  return 0;
}
