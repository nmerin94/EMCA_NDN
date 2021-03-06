/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

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

NS_LOG_COMPONENT_DEFINE("ndn.WifiExample");

// {
//   // NS_LOG_DEBUG ("Create custom network device " << node->GetId ());
//   Ptr<ndn::NetDeviceFace> face = CreateObject<ndn::MyNetDeviceFace> (node, device);
//   ndn->AddFace (face);
//   return face;
// }



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


int
main(int argc, char* argv[])
{
  // disable fragmentation
  Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
  Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode",
                     StringValue("OfdmRate24Mbps"));

  CommandLine cmd;
  cmd.Parse(argc, argv);

  //////////////////////
  //////////////////////
  //////////////////////
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

  /*MobilityHelper mobility;
  mobility.SetPositionAllocator("ns3::RandomBoxPositionAllocator", "X", PointerValue(randomizer),
                                "Y", PointerValue(randomizer), "Z", PointerValue(randomizer));

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

   */
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 15.0, 0.0));
/*
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (10.0, 10.0, 0.0));
  positionAlloc->Add (Vector (20.0, 20.0, 0.0));
  positionAlloc->Add (Vector (30.0, 30.0, 0.0));
  positionAlloc->Add (Vector (40.0, 40.0, 0.0));
*/
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

    // set mobility
  MobilityHelper mobilitas;
  ObjectFactory pos;
  pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
  pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=35.0]"));
  pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=35.0]"));
  Ptr<PositionAllocator> posAlloc = pos.Create()->GetObject<PositionAllocator>();
  mobilitas.SetMobilityModel("ns3::RandomWaypointMobilityModel", 
                            "Speed", StringValue ("ns3::UniformRandomVariable[Min=10|Max=20]"),
                            "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=6.0]"),
                            "PositionAllocator", PointerValue(posAlloc));
  mobilitas.SetPositionAllocator (posAlloc);
  
  NodeContainer nodes;
  nodes.Create(6);

  ////////////////
  // 1. Install Wifi
  NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, nodes);

  // 2. Install Mobility model
  mobility.Install(nodes.Get(0));
  mobility.Install(nodes.Get(1));
  mobility.Install(nodes.Get(5));
  mobilitas.Install(nodes.Get(4));
  mobilitas.Install(nodes.Get(2));
  mobilitas.Install(nodes.Get(3));
  
  //mobility.Install(nodes);

  // 3. Install NDN stack
  NS_LOG_INFO("Installing NDN stack");
  ndn::StackHelper ndnHelper;
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.Install(nodes);

  // Set BestRoute strategy
  /*for(int i = 1; i<4; i++)
   ndn::StrategyChoiceHelper::Install(nodes.Get(i), "/", "/localhost/nfd/strategy/multicast");
  ndn::StrategyChoiceHelper::Install(nodes.Get(1), "/", "/localhost/nfd/strategy/best-route");
  ndn::StrategyChoiceHelper::Install(nodes.Get(4), "/", "/localhost/nfd/strategy/best-route");
  */
   ndn::StrategyChoiceHelper::InstallAll("/","/localhost/nfd/strategy/multicast" );     //IF COMMENTED, INTEREST WONT PROPOGATE
  // 4. Set up applications
  NS_LOG_INFO("Installing Applications");

  /*ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.Install(nodes);
  ndnGlobalRoutingHelper.AddOrigins("/test", nodes.Get(4));
  ndn::GlobalRoutingHelper::CalculateRoutes();*/


  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  ndn::AppHelper relayhelper("ns3::ndn::Relay");

  
  consumerHelper.SetPrefix("/test/prefix");
  consumerHelper.SetAttribute("Frequency", DoubleValue(10));
  consumerHelper.Install(nodes.Get(0));
  consumerHelper.Install(nodes.Get(1));
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  //ndnGlobalRoutingHelper.AddOrigins("/test/prefix", nodes.Get(4));
  producerHelper.SetPrefix("/test");
  producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
  producerHelper.Install(nodes.Get(5));
  relayhelper.SetPrefix("/");
  for(int i = 2; i < 4; i++){
    relayhelper.Install(nodes.Get(i));
  }

  //Calculate routes for FIB
  
  ////////////////
  

  Simulator::Stop(Seconds(30.0));

  AnimationInterface anim ("ndn_wireless_NetAnimationOutput.xml");
  anim.SetConstantPosition(nodes.Get(0), 0, 15);
  anim.SetConstantPosition (nodes.Get(1), 0, 0);
  //anim.SetConstantPosition (nodes.Get(2), 10, 10);
  //anim.SetConstantPosition (nodes.Get(3), 20, 20);
  //anim.SetConstantPosition (nodes.Get(4), 30, 30);
  anim.SetConstantPosition (nodes.Get(5), 30, 30);
  
  //anim.EnablePacketMetadata (true);
  ndn::L3RateTracer::Install(nodes.Get(0), "rate-trace0.txt", Seconds(15));
  ndn::L3RateTracer::Install(nodes.Get(1), "rate-trace1.txt", Seconds(15));
  PcapWriter trace("ndn-simple-trace.pcap");
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/MacTx",
                                MakeCallback(&PcapWriter::TracePacket, &trace));


  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}