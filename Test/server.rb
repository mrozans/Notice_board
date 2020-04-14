require 'socket'

include Socket::Constants

socket = Socket.new(AF_INET, SOCK_STREAM, 0)
sockaddr = Socket.sockaddr_in(2001, '0.0.0.0')
socket.bind(sockaddr)
socket.listen(5)

loop do
  client, client_addrinfo = socket.accept

  puts "- - - - - #{Time.now}", client.inspect, client_addrinfo.inspect

  begin
    pair = client.recvfrom_nonblock(1024)
  rescue IO::WaitReadable
    IO.select([client])
    retry
  end

  puts pair.first.inspect
end

socket.close