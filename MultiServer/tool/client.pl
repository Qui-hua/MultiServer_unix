use IO::Socket;
use Time::HiRes qw( gettimeofday );
use strict;
use Cwd;
use POSIX qw(strftime);
#sub send_data
#{
for(my $i=0;$i<100;$i++)
{

	my ($name) = $ARGV[0];	
	my $host = '127.0.0.1'; # Host
	my $client = IO::Socket::INET->new(
		PeerAddr => $host,
		PeerPort => 5429
	);
	 $client ? print "$name Connected to $host.\n\n" : die "Cannot connect to $host $!";
	
	my $left = 0;
	my $buf = 0;
    my $senddata="0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
	
	my ($seconds, $microseconds) = gettimeofday;
	print  "$name START SEND $seconds $microseconds "."-\n";
	 
	$left = syswrite $client,$senddata, 100;
	print  $name," send ",$left,"-\n";
	
	$left = sysread $client,$buf, 100;
	print  $name," recv ",$left,"-\n";
	
	$client->close();
}