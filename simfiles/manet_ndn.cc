/* Simulation file created as part of ADVANCED COMPUTER NETWORKS project on MANET using NDNSIM 


Authors :
Nisha Merin Jacob 2018H1030129P
Anupa Ann Jacob  2018H1030142P


*/
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/ndnSIM-module.h"
using namespace std;
namespace ns3 {
	NS_LOG_COMPONENT_DEFINE("ndn.MANET");

	 NodeContainer nodes;


// CLASS TO ENABLE PCAP TRACE FILE

	class PcapWriter {
	public:
	  PcapWriter(const std::string& file)
	  {
	    PcapHelper helper;
	    m_pcap = helper.CreateFile(file, std::ios::out, PcapHelper::DLT_PPP);
	  }

	  void
	  TracePacket(Ptr<const Packet> packet)
	  {
	    static PppHeader pppHeader;
	    pppHeader.SetProtocol(0x0077);

	    m_pcap->Write(Simulator::Now(), pppHeader, packet);
	  }

	private:
	  Ptr<PcapFileWrapper> m_pcap;
	};



	void buildapps(uint32_t n_nodes, uint32_t freq)
	{
		NS_LOG_INFO("Installing Applications");
		ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		ndn::AppHelper relayhelper("ns3::ndn::Relay");

  
		consumerHelper.SetPrefix("/test/prefix");
		consumerHelper.SetAttribute("Frequency", DoubleValue(freq));
		consumerHelper.Install(nodes.Get(0));
		consumerHelper.Install(nodes.Get(1));
		ndn::AppHelper producerHelper("ns3::ndn::Producer");
  		//ndnGlobalRoutingHelper.AddOrigins("/test/prefix", nodes.Get(4));
		producerHelper.SetPrefix("/test");
		producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
		producerHelper.Install(nodes.Get(n_nodes - 1));
		relayhelper.SetPrefix("/");
		for(int i = 2; i < n_nodes - 1; i++){
    		relayhelper.Install(nodes.Get(i));
		}
	}



	void static_mob(uint32_t n_nodes)
	{
		MobilityHelper mobility;
  		mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                  "X", StringValue ("0.0"),
                                  "Y", StringValue ("0.0"),
                                  "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  		mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  		mobility.Install(nodes);

	}




	void moving_mob(uint32_t n_nodes)
	{

		MobilityHelper mobility;
  		mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                  "X", StringValue ("0.0"),
                                  "Y", StringValue ("0.0"),
                                  "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  		mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  		mobility.Install(nodes.Get(0));
  		mobility.Install(nodes.Get(1));
  		mobility.Install(nodes.Get(n_nodes - 1));

  		MobilityHelper mobilitas;
		ObjectFactory pos;
		pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
		pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=30.0]"));
		pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=30.0]"));
		Ptr<PositionAllocator> posAlloc = pos.Create()->GetObject<PositionAllocator>();
		mobilitas.SetMobilityModel("ns3::RandomWaypointMobilityModel", 
                            "Speed", StringValue ("ns3::UniformRandomVariable[Min=10|Max=20]"),
                            "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"),
                            "PositionAllocator", PointerValue(posAlloc));
  		mobilitas.SetPositionAllocator (posAlloc);
  		for(int i = 2; i < n_nodes - 1; i++)
  			mobilitas.Install(nodes.Get(i));
  
	}





	int	main (int argc, char *argv[])
 	{
		uint32_t mob = 1;  // Without Mobility
		uint32_t n_nodes = 6;
		uint32_t freq = 10
		// Getting Choice via Command line
		/*
		./waf --run "scratch/manet_ndn --PrintHelp" will print help.
		To set mobility model as Random waypoint, use  
		./waf --run "scratch/manet_ndn --mob=2"  
		*/

		CommandLine cmd;
		cmd.AddValue("mob", "1: No Mobility, 2: Mobility", mob);
		cmd.AddValue("n_nodes", "Number of nodes", n_nodes);
		cmd.AddValue("freq", "Interest frequency", freq);
		cmd.Parse (argc, argv);

		nodes.Create(n_nodes);

		WifiHelper wifi = WifiHelper::Default();
		// wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
		wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
		wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
                               StringValue("OfdmRate24Mbps"));


		YansWifiChannelHelper wifiChannel; // = YansWifiChannelHelper::Default ();
		wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
		//wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
		//wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
		wifiChannel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel","Exponent", DoubleValue (3));
		// YansWifiPhy wifiPhy = YansWifiPhy::Default();
		YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
		wifiPhyHelper.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
		wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
		wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));
		wifiPhyHelper.SetChannel(wifiChannel.Create());

		NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
		wifiMacHelper.SetType("ns3::AdhocWifiMac");

		Ptr<UniformRandomVariable> randomizer = CreateObject<UniformRandomVariable>();
		randomizer->SetAttribute("Min", DoubleValue(0));
		randomizer->SetAttribute("Max", DoubleValue(100));

		NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, nodes);

		if(mob == 1 )
			static_mob(n_nodes);
		else
			moving_mob(n_nodes);

		
		// 3. Install NDN stack
		NS_LOG_INFO("Installing NDN stack");
		ndn::StackHelper ndnHelper;
		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
		ndnHelper.SetDefaultRoutes(true);
		ndnHelper.Install(nodes);
		ndn::StrategyChoiceHelper::InstallAll("/","/localhost/nfd/strategy/multicast" );
		// Install apps
		NS_LOG_INFO("Installing Applications");
		buildapps(n_nodes, freq);

		Simulator::Stop(Seconds(31.0));
		AnimationInterface anim ("ndn_wireless_NetAnimationOutput.xml");
		anim.SetConstantPosition(nodes.Get(0), 0, 15);
		anim.SetConstantPosition (nodes.Get(1), 0, 0);
		anim.SetConstantPosition (nodes.Get(n_nodes-1), 30, 30);
		ndn::L3RateTracer::Install(nodes.Get(0), "rate-trace0.txt", Seconds(15));
		ndn::L3RateTracer::Install(nodes.Get(1), "rate-trace1.txt", Seconds(15));
		PcapWriter trace("ndn-simple-trace.pcap");
		Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/MacTx",
                                					MakeCallback(&PcapWriter::TracePacket, &trace));
		Simulator::Run();
  		Simulator::Destroy();
  		return 0;
	}
}//namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}