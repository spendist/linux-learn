#!/usr/bin/perl

use strict;

use IO::Socket;
use Data::Dumper;

my $dt=DateTime::Format::Strptime->new(pattern=>'%Y-%m-%d %H:%M:%S');
print $dt;

$toPhone='13550225979';
$toContent="这是一个测试perl的信息。请检查中英文混合输入。\r\n这是第二行\r\n";


$Packet="<eit><TrCode>700001</TrCode><MsgType>3</MsgType><Mobile>$toPhone</Mobile><RetScript>$toContent</RetScript><Operator>111111</Operator><Branch>10</Branch></eix>";

print $Packet;


my $lsocket=IO:Socket::INET->new(PeerAddr='192.168.2.3',
                                 PeerPort='4321',
								 Proto='tcp' );

my $msg=$lsocket->send('haha test message by protol');

print "Send $msg\n";

my $getmsg;
$lsocket->recv($getmsg,30);
print "Get Msg=[ $getmsg ] \n";

