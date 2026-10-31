
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PointToPointExample");

int main(int argc, char *argv[])
{
    // Create two nodes
    NodeContainer nodes;
    nodes.Create(2);

    // Configure point-to-point connection
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute(
        "DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute(
        "Delay", StringValue("2ms"));

    // Install network devices
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // Install Internet stack
    InternetStackHelper stack;
    stack.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces;
    interfaces = address.Assign(devices);

    // UDP Echo Server on Node 1
    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps =
        echoServer.Install(nodes.Get(1));

    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // UDP Echo Client on Node 0
    UdpEchoClientHelper echoClient(
        interfaces.GetAddress(1), 9);

    echoClient.SetAttribute(
        "MaxPackets", UintegerValue(5));
    echoClient.SetAttribute(
        "Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute(
        "PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps =
        echoClient.Install(nodes.Get(0));

    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

LogComponentEnable(
    "UdpEchoClientApplication", LOG_LEVEL_INFO);

LogComponentEnable(
    "UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Run simulation
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
