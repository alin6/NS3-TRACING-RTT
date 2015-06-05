/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Skeleton code wireless-tcp-bulk-send.cc
 */

#include <string>
#include <iostream>
#include "ns3/aodv-module.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/athstats-helper.h"
#include "ns3/netanim-module.h"
#include "../src/internet/model/tcp-socket-base-1.h"//<! adds global variable to track traced node's ID


using namespace ns3;
using namespace std;


NS_LOG_COMPONENT_DEFINE ("AODVRtt");

bool firstCwnd = true;
bool firstSshThr = true;
bool firstRtt = true;
bool firstRto = true;
bool firstRttVar = true;
bool firstRealRtt = true;
bool firstDelta = true;
bool anim_meta = true;


/*****************************************************************************/
/* OUTPUT STREAMS                                                            */
/*****************************************************************************/

Ptr<OutputStreamWrapper> cWndStream;
Ptr<OutputStreamWrapper> ssThreshStream;
Ptr<OutputStreamWrapper> rttStream;
Ptr<OutputStreamWrapper> rtoStream;
Ptr<OutputStreamWrapper> rttvarStream;
Ptr<OutputStreamWrapper> rrttStream;
Ptr<OutputStreamWrapper> deltaStream;

uint32_t cWndValue;
uint32_t ssThreshValue;


/*****************************************************************************/
/* TRACE PATHS                                                               */
/*****************************************************************************/

std::string CwndPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/CongestionWindow");
std::string ssThreshPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/SlowStartThreshold");
std::string RealRttPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/realRTT");
std::string estRttPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTT");
std::string RtoPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTO");
std::string RttVarPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTTvar");
std::string DeltaPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/Delta");


/*****************************************************************************/
/*CALLBACK FUNCTIONS FOR TRACES                                               */
/*****************************************************************************/
static void
CwndTracer (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd)
    {
      *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd = false;
    }
  *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue = newval;

  if (!firstSshThr)
    {
      *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
    }
}

static void
SsThreshTracer (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr)
    {
      *ssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr = false;
    }
  *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue = newval;

  if (!firstCwnd)
    {
      *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue << std::endl;
    }
}

static void
RttTracer (Time oldval, Time newval)
{
  if (firstRtt)
    {
      *rttStream->GetStream () << m_nodeID << " "  <<  "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt = false;
    }
  *rttStream->GetStream () << m_nodeID << " " <<  Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer (Time oldval, Time newval)
{
  if (firstRto)
    {
      *rtoStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto = false;
    }
  *rtoStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}
static void
RttVarTracer (Time oldval, Time newval)
{
  if (firstRttVar)
    {
      *rttvarStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRttVar = false;
    }
  *rttvarStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}




static void
RealRttTracer (Time oldval, Time newval)
{
  if (firstRealRtt)
    {
      *rrttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRealRtt = false;
    }
  *rrttStream->GetStream ()  << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}


static void
DeltaTracer (Time oldval, Time newval)
{
  if (firstDelta)
    {
      *deltaStream->GetStream () <<  "0.0 " << oldval.GetSeconds () << std::endl;
      firstDelta = false;
    }
  *deltaStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}



/*****************************************************************************/

/*****************************************************************************/
/*SETTING UP FILES FOR STORING TRACE VALUES                                  */
/*****************************************************************************/
static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext (CwndPath, MakeCallback (&CwndTracer));
}

static void
TraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext (ssThreshPath, MakeCallback (&SsThreshTracer));
}

static void
TraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext (estRttPath, MakeCallback (&RttTracer));
}

static void
TraceRto (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext (RtoPath, MakeCallback (&RtoTracer));
}
static void
TraceRealRtt (std::string rrtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rrttStream = ascii.CreateFileStream (rrtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext (RealRttPath, MakeCallback (&RealRttTracer));
}
static void
TraceRttVar (std::string rttvar_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttvarStream = ascii.CreateFileStream (rttvar_tr_file_name.c_str ());
  Config::ConnectWithoutContext (RttVarPath, MakeCallback (&RttVarTracer));
}
static void
TraceDelta (std::string delta_tr_file_name)
{
  AsciiTraceHelper ascii;
  deltaStream = ascii.CreateFileStream (delta_tr_file_name.c_str ());
  Config::ConnectWithoutContext (DeltaPath, MakeCallback (&DeltaTracer));
}


void
PhyTxTrace (std::string context, Ptr<const Packet> packet, WifiMode mode, WifiPreamble preamble, uint8_t txPower)
{
      std::cout << "PHYTX mode=" << mode << " " << *packet << std::endl;
}


int
main (int argc, char *argv[])
{
  uint32_t maxBytes = 300; //megaBytes
  double SimTime = 10.0;
  std::string transport_prot = "TcpWestwood";
  //NS_LOG_INFO ("error probabiltiy 0.0");
  double error_p = 0.0;

  uint32_t run = 0;
  uint32_t seed = 1;
  double range = 250;
  /*defines the interval in with additional nodes can start transmitting*/
  /*DEFUALT 2 Seconds*/
  double delay_start_interval = 2.0;
  /*defines number of nodes to to be added to transmitting set every interval*/
  /*DEFUALT 3nodes*/
  uint16_t num_add_trans_node = 3;
  /*the number for flows*/
  uint16_t num_flows = 1;

  std::string mobility_model = "RandomWalk";
  std::string placement = "Grid";
  std::string traffic_pattern = "Burst";

  /*LOG COMPONENTS*/
  LogComponentEnable("AODVRtt", LOG_LEVEL_INFO);
  //  LogComponentEnable("ConstantRateWifiManager", LOG_LEVEL_INFO);
  //  LogComponentEnable ("BulkSendApplication", LOG_LEVEL_INFO);
  //  LogComponentEnable ("TcpSocketBase", LOG_LEVEL_LOGIC);

  std::string traceFile = "wireless.ns_movements";
  std::string tr_file_name = "";
  std::string cwnd_tr_file_name = "";
  std::string ssthresh_tr_file_name = "";
  std::string rtt_tr_file_name = "wireless.rtt";
  std::string rto_tr_file_name = "";
  std::string rwin_tr_file_name = "";
  std::string rttvar_tr_file_name = "";
  std::string rrtt_tr_file_name = "wireless.rrtt";
  std::string delta_tr_file_name = "wireless.delta";



  CommandLine cmd;
  cmd.AddValue ("transport_prot", "Transport protocol to use: TcpTahoe, TcpReno, TcpNewReno, TcpWestwood, TcpWestwoodPlus ", transport_prot);
  cmd.AddValue ("MBytes", "maximum number of megabytes during the transfer ", maxBytes);
  cmd.AddValue ("error_p", "Packet error rate", error_p);
  cmd.AddValue ("seed", "set seed repeatable runs", seed);
  cmd.AddValue ("run", "Run index (for setting repeatable seeds)", run);
  cmd.AddValue ("duration", "Time to allow flows to run in seconds", SimTime);
  cmd.AddValue ("tr_name", "Name of output trace file", tr_file_name);
  cmd.AddValue ("rwnd_tr_name", "Name of output trace file", rwin_tr_file_name);
  cmd.AddValue ("cwnd_tr_name", "Name of output trace file", cwnd_tr_file_name);
  cmd.AddValue ("ssthresh_tr_name", "Name of output trace file", ssthresh_tr_file_name);
  cmd.AddValue ("rtt_tr_name", "Name of output trace file for ESIMATED RTT", rtt_tr_file_name);
  cmd.AddValue ("rto_tr_name", "Name of output trace file", rto_tr_file_name);
  cmd.AddValue ("rttvar_tr_name", "Name of output trace file", rttvar_tr_file_name);
  cmd.AddValue ("rrtt_tr_name", "Name of output trace file for REAL RTT", rrtt_tr_file_name);
  cmd.AddValue ("delta_tr_name", "Name of output trace file", delta_tr_file_name);
  cmd.AddValue ("range", "Cutoff range of transmission ", range);
  cmd.AddValue ("num_flows", "Number of flows", num_flows);
  cmd.AddValue ("delay_start_interval", "Delay in seconds Interval between starting transmissions between groups of nodes", delay_start_interval);
  cmd.AddValue ("num_add_trans_node", "Number of flows to be started at every interval", num_add_trans_node);
  cmd.AddValue ("anim_meta", "Enabe/Disable netanim packet metadata", anim_meta);
  cmd.AddValue ("mobility_model", "Mobility Mode (RandomWalk|ConstantPosition|RandomWaypoint) Default:", mobility_model);
  cmd.AddValue ("placement", "Node Placement Model (Grid|UniformDisc) Default:", placement);
  cmd.AddValue ("traffic_pattern", "Traffic Patter (Constant| Increasing| Burst) Default:", traffic_pattern);

  cmd.Parse (argc, argv);


  SeedManager::SetSeed (seed);
  SeedManager::SetRun (run);


  /*TCP CONFIGURATION*/
  Config::SetDefault ("ns3::TcpSocketBase::WindowScaling", BooleanValue (true));
  if (transport_prot.compare ("TcpTahoe") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpTahoe::GetTypeId ()));
    }
  else if (transport_prot.compare ("TcpReno") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpReno::GetTypeId ()));
    }
  else if (transport_prot.compare ("TcpNewReno") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId ()));
    }
  else if (transport_prot.compare ("TcpWestwood") == 0)
    { // the default protocol type in ns3::TcpWestwood is WESTWOOD
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
    }
  else if (transport_prot.compare ("TcpWestwoodPlus") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
      Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
    }
  else
    {
      NS_LOG_DEBUG ("Invalid TCP version");
      exit (1);
    }




//
// Explicitly create the nodes required by the topology (shown above).
//
  NS_LOG_INFO ("Create nodes.");
  uint16_t num_nodes=(2*num_flows);

  NodeContainer nodes;
  nodes.Create(num_nodes);



  NS_LOG_INFO ("Total Number of Nodes: " << NodeList::GetNNodes());
  NS_LOG_INFO ("Total Number Flows: " << num_flows);
  if (num_flows > num_add_trans_node) {
    NS_LOG_INFO ("===================================================================");
    NS_LOG_INFO ("first " << num_add_trans_node << " flows will be started at time 0.0");
    NS_LOG_INFO ("upto to  " << num_add_trans_node << "additional flows will start at " << delay_start_interval << " second(s) intervals");
  }
  NS_LOG_INFO ("===================================================================");

  NS_LOG_INFO ("TCP Flavor: " << transport_prot);
  NS_LOG_INFO ("Simulation Duration: " << SimTime);
  NS_LOG_INFO ("Configuring error model. Error Probability: " << error_p);
  NS_LOG_INFO ("===================================================================");
  NS_LOG_INFO ("Mobility Model: " << mobility_model);
  NS_LOG_INFO ("Node Placement Model: " << placement);
  NS_LOG_INFO ("===================================================================");

  // Configure the error model
  // Here we use RateErrorModel with packet error rate
  Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable> ();
  uv->SetStream (50);
  RateErrorModel error_model;
  error_model.SetRandomVariable (uv);
  error_model.SetUnit (RateErrorModel::ERROR_UNIT_PACKET);
  error_model.SetRate (error_p);


  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();

  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));

  //
  // Explicitly create the point-to-point link required by the topology (shown above).
  NS_LOG_INFO ("Create channels.");



  // Mac Layer


  NS_LOG_INFO ("Propataion Delay Model: Random Propagation Delay Model");
  wifiChannel.SetPropagationDelay ("ns3::RandomPropagationDelayModel");

  /*CUT OFF RANGE*/
  NS_LOG_INFO ("Propagation range: " << range);
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(range));
  //MORE MODELS AT https://www.nsnam.org/docs/release/3.15/doxygen/group__propagation.html
  NS_LOG_INFO ("===================================================================");



  //wifiHelper.SetRemoteStationManager("ns3::MinstrelWifiManager");

  NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);



  NS_LOG_INFO ("Install Internet Stack.");

  //
  // Install the internet stack on the nodes
  //

  AodvHelper aodv;
  InternetStackHelper internet;
  internet.SetRoutingHelper (aodv);
  internet.Install (nodes);

  //
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  //
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interface = ipv4.Assign (devices);




  // mobility.
  MobilityHelper mobility;
  if (placement.compare ("Grid") == 0)
  {
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (35.0),
                                  "MinY", DoubleValue (40.0),
                                  "DeltaX", DoubleValue (5.0),
                                  "DeltaY", DoubleValue (10.0),
                                  "GridWidth", UintegerValue (5),
                                  "LayoutType", StringValue ("RowFirst"));
 }else if (placement.compare ("UniformDisc") == 0)
 {
   double radius = 20.0;
   mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
                                       "X", DoubleValue (50.0),
                                       "Y", DoubleValue (50.0),
                                       "rho", DoubleValue (radius));
  }
  // ListPositionAllocator, RandomRectanglePositionAllocator, RandomDiscPositionAllocator Can also be implemented



  /*SET MOBILITY MODEL*/
  /*MORE https://www.nsnam.org/docs/models/html/mobility.html*/
  if (mobility_model.compare ("RandomWalk") == 0)
  {
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                                /*DECLARE DISTNACE OR TIME MODE*/
                                "Mode", StringValue ("Time"),
                                /*DEFINE TIME DURATION BEFORE CHANGE IN SPEED AND DIRECTION*/
                                "Time", StringValue ("2s"),
                                //https://www.nsnam.org/doxygen/classns3_1_1_constant_random_variable.html#details
                                "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]")
                                /*TO ASSIGN MOVEMENT BOUNDS*/
                                //,"Bounds", RectangleValue (Rectangle (-50, 50, -25, 50))
                                );
  } else if (mobility_model.compare ("ConstantPosition") == 0)
  {
    mobility.SetMobilityModel ("ns3::ConstantPosition");
  } else if (mobility_model.compare ("RandomWaypoint") == 0)
  {

    ObjectFactory position;
    position.SetTypeId ("ns3::RandomRectanglePositionAllocator");
    position.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=250.0]"));
    position.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=250.0]"));
    Ptr<PositionAllocator> PositionAlloc = position.Create ()->GetObject<PositionAllocator> ();

    mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
                            //GENERATE RANDSOME SPEED
                           "Speed", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=20.0]"),
                           //GENERATE RANDOM PAUSES
                           //MORE REPEATABLE SEED[Constant=2.0]
                           "Pause", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=3.0]"),
                           "PositionAllocator", PointerValue (PositionAlloc)
                           );
  }

  mobility.Install (nodes);





  NS_LOG_INFO ("Create Applications.");

  //
  // Create a BulkSendApplication
  //
  uint16_t port = 8080;  // well-known echo port number

  ApplicationContainer sourceApps;


    NS_LOG_INFO ("Traffic Pattern: Burst" );
    // Create the OnOff applications to send TCP

    for (int i = 0; i < num_flows; i++)
      {
NS_LOG_INFO ("loop 1" );
        OnOffHelper source("ns3::TcpSocketFactory",Address ());
        NS_LOG_INFO ("loop 2" );


        source.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
        //source.SetAttribute ("OffTime", RandomVariableValue (ConstantVariable (0)));
        source.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));

        NS_LOG_INFO ("loop 3" );

        AddressValue remoteAddress(InetSocketAddress (interface.GetAddress ((2*i)+1), port));
        source.SetAttribute ("Remote", remoteAddress);
        sourceApps = source.Install (nodes.Get (2*i));
        sourceApps.Start (Seconds (0.1));
        sourceApps.Stop (Seconds (SimTime));
      }
      ApplicationContainer sinkApp;
      PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
      for (int i = 0; i < num_flows; i++)
      {
        sinkApp = sink.Install (nodes.Get ((2*i)+1));
        sinkApp.Start (Seconds (0.1));
        sinkApp.Stop (Seconds (SimTime));
      }

  if (true)
    {
      //AsciiTraceHelper ascii;
      //wifiHelper.EnableAsciiAll (ascii.CreateFileStream ("wireless_rtt.tr"));
      wifiPhy.EnablePcapAll ("aodv_rtt", true);
    }
//
// Now, do the actual simulation.
//
  // Setting up Animation Interface nicely

    AnimationInterface anim ("aodv_rtt.xml");
    /*Assign Label for access point*/
    anim.UpdateNodeDescription (nodes.Get (0), "wireless access point");
    /*base station color*/
    anim.UpdateNodeColor(nodes.Get (0), 0, 0,255);

    /*Assign Label for mobile wireless stations*/
    for (int i = 0; i < num_flows; i++)
      {
        char str[18];
        sprintf(str,"wireless station %d", (i+1));
        anim.UpdateNodeDescription (nodes.Get (i), str);
        /*enable packet metadata*/
        anim.EnablePacketMetadata(anim_meta);
      }

  /*Residual code, keeps trace of total databytes recieved at sink*/
  Ptr<PacketSink> sink2 = DynamicCast<PacketSink> (sinkApp.Get (0));

/********************************************************************************/
/*HOOKS FOR STARTING TRACES *****************************************************/
/********************************************************************************/
double trace_start=0.5;
if (tr_file_name.compare ("") != 0)
  {
    std::ofstream ascii;
    Ptr<OutputStreamWrapper> ascii_wrap;
    ascii.open (tr_file_name.c_str ());
    ascii_wrap = new OutputStreamWrapper (tr_file_name.c_str (), std::ios::out);
    internet.EnableAsciiIpv4All (ascii_wrap);
  }
if (cwnd_tr_file_name.compare ("") != 0)
  {
    Simulator::Schedule (Seconds (trace_start), &TraceCwnd, cwnd_tr_file_name);
  }

if (ssthresh_tr_file_name.compare ("") != 0)
  {
    Simulator::Schedule (Seconds (trace_start), &TraceSsThresh, ssthresh_tr_file_name);
  }
/*****estimate RTT**************************************************************/
if (rtt_tr_file_name.compare ("") != 0)
  {
    Simulator::Schedule (Seconds (trace_start), &TraceRtt, rtt_tr_file_name);
  }

if (rto_tr_file_name.compare ("") != 0)
  {
    Simulator::Schedule (Seconds (trace_start), &TraceRto, rto_tr_file_name);
  }

/*****RTT VARIANCE**************************************************************/
if (rttvar_tr_file_name.compare ("") != 0)
  {
    Simulator::Schedule (Seconds (trace_start), &TraceRttVar, rttvar_tr_file_name);
  }
/*****REAL RTT**************************************************************/
if (rrtt_tr_file_name.compare ("") != 0)
  {
    Simulator::Schedule (Seconds (trace_start), &TraceRealRtt, rrtt_tr_file_name);
  }

  /*****Delta**************************************************************/
if (delta_tr_file_name.compare ("") != 0)
  {
    Simulator::Schedule (Seconds (trace_start), &TraceDelta, delta_tr_file_name);
  }


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (SimTime));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");


  std::cout << "Total Bytes Received: " << sink2->GetTotalRx () << std::endl;
  std::cout << "Total Throughput: " << (sink2->GetTotalRx ()*8)/(1000000*SimTime) << " Mbps" << std::endl;
}
