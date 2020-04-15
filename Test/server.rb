require 'socket'
require 'thread'

include Socket::Constants

def handle_connection(socket, service_type)
  client, client_addrinfo = socket.accept
  
  puts "-> #{service_type}: #{Time.now}", client.inspect, client_addrinfo.inspect

  begin
    pair = client.recvfrom_nonblock(1024)
  rescue IO::WaitReadable
    IO.select([client])
    retry
  end

  puts pair.first.inspect
end

threads = []

# IPv6 server

socket_ipv6 = Socket.new(AF_INET6, SOCK_STREAM, 0)
sockaddr_ipv6 = Socket.sockaddr_in(2000, '*')
socket_ipv6.bind(sockaddr_ipv6)
socket_ipv6.listen(5)

threads << Thread.new do
  loop do
    handle_connection(socket_ipv6, 'ipv6_thread')
  end
end

# IPv4 server

socket_ipv4 = Socket.new(AF_INET, SOCK_STREAM, 0)
sockaddr_ipv4 = Socket.sockaddr_in(2000, '0.0.0.0')
socket_ipv4.bind(sockaddr_ipv4)
socket_ipv4.listen(5)

threads << Thread.new do
  loop do
    handle_connection(socket_ipv4, 'ipv4_thread')
  end
end

threads.each { |t| t.join }

socket_ipv6.close
socket_ipv4.close