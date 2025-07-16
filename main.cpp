#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <map>
#include <fcntl.h>
#include <errno.h>

using namespace std;

bool is_host_up(const string& ip_addr)
{
  string cmd = "ping -c 1 -w 1 " + ip_addr + " > /dev/null 2>&1";

  return system(cmd.c_str()) == 0;
}

string resolve_hostname(const char* hostname)
{
  struct in_addr addr;
  if (inet_pton(AF_INET, hostname, &addr) == 1)
  {
    return string(hostname);
  }

  struct addrinfo hints{}, *res;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int status = getaddrinfo(hostname, nullptr, &hints, &res);
  if(status != 0)
  {
    cerr << "getaddrinfo error: " << gai_strerror(status) << endl;
    return "";
  }

  char ipstr[INET_ADDRSTRLEN];
  struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
  inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));

  freeaddrinfo(res);

  return string(ipstr);
}

int scan_ports(const char* ip_addr)
{
  string resolved_ip = resolve_hostname(ip_addr);
  const char* const_ip_addr = resolved_ip.c_str();

  if(is_host_up(const_ip_addr) == true)
  {
    cout << "Host is up!" << endl;
  }
  else {
    cout << "Host is down. Exiting..." << endl;
    return 0;
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;

  // COnverting IPv4 from string to binary
  if (inet_pton(AF_INET, const_ip_addr, &serv_addr.sin_addr) <= 0) 
  {
    cerr << "Invalid address" << endl;
    return -1;
  }

  map<int, string> port_services = {
     {21, "FTP"}, {22, "SSH"}, {23, "Telnet"},
     {25, "SMTP"}, {53, "DNS"}, {80, "HTTP"},
     {110, "POP3"}, {143, "IMAP"}, {443, "HTTPS"},
     {3306, "MySQL"}, {8080, "HTTP (Alt)"}}; 

  for(int i = 1; i <= 65535; i++)
  {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
      cerr << "Socket creation error" << endl;
      return -1;
    }

    serv_addr.sin_port = htons(i);

    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    int res = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(res < 0 && errno != EINPROGRESS)
    {
      close(sock);
      continue;
    }

    fd_set wait_set;
    FD_ZERO(&wait_set);
    FD_SET(sock, &wait_set);
    struct timeval select_timeout;
    select_timeout.tv_sec = 1;
    select_timeout.tv_usec = 0;

    res = select(sock + 1, nullptr, &wait_set, nullptr, &select_timeout);
    if(res <= 0)
    {
      close(sock);
      continue;
    }
    else 
    {
      int so_error;
      socklen_t len = sizeof(so_error);
      getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
      if(so_error != 0)
      {
        close(sock);
        continue;
      }

      fcntl(sock, F_SETFL, flags);

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
      struct timeval recv_timeout;
      recv_timeout.tv_sec = 1;
      recv_timeout.tv_usec = 0;
      setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recv_timeout, sizeof(recv_timeout));

      char buffer[100] = {0};
      int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
      if(bytes > 0)
      {
        buffer[bytes] = '\0';
        cout << buffer << endl;
      }
      else 
      {
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
  cout << "Enter the hostname/IP address to scan: ";
  cin >> ip_addr;
  
  scan_ports(ip_addr.c_str());

  return 0;
}
