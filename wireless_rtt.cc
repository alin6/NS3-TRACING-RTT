
 /* aodv_rtt.cc                                                                                */
 /* AUTHOR: ALAN LIN                                                                           */
 /* DONOR CODE wireless-tcp-bulk-send.cc                                                       */
 /*                                                                                            */
 /* WIRELESS AP TOPOLOGY WITH THE ABILITY SCALE UP TO NUMBEROUS MOBILE/STATIONARY              */
 /* NODES ON THE NETWORK. THIS CODE ALLOWS FOR VARIOUS PLACEMENT MODELS, MOBILITY MODELS       */
 /* AS WELL AS TRAFFIC PATTERNS.                                                               */
 /*                                                                                            */
 /* THE NODE REPLIES ON A MODIEFIED rtt-estimator.cc/h AS WELL AS tcp-socket-base.cc/H         */
 /* TO PROVIDE TRACING OF DELTA AND REAL RTT VALUES.                                           */
 /*****************************************************************************************/
#include <string>
#include <iostream>
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


NS_LOG_COMPONENT_DEFINE ("WirelessRtt");
/* FOR INITIALIZING TRACES */
bool firstCwnd = true;
bool firstSshThr = true;
bool firstRtt = true;
bool firstRto = true;
bool firstRttVar = true;
bool firstRealRtt = true;
bool firstDelta = true;
bool anim_meta = true;
bool firstFixed = true;


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
Ptr<OutputStreamWrapper> fixedStream;

uint32_t cWndValue;
uint32_t ssThreshValue;


/*****************************************************************************/
/* TRACE PATHS                                                               */
/* THE ASTERICKS ARE USED IN THE PATH AS WILDCARD TO TRACE ALL NODES/SOCKETS */
/*****************************************************************************/

std::string CwndPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/CongestionWindow");
std::string ssThreshPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/SlowStartThreshold");
std::string RealRttPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/realRTT");
std::string estRttPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTT");
std::string RtoPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTO");
std::string RttVarPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTTvar");
std::string DeltaPath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/Delta");
/***FIXED SHARE ESTIMATE****************************************************************/
std::string FixedSharePath("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/FixedShare");


/********************************************************************************/
/*CALLBACK FUNCTIONS FOR TRACES                                                 */
/*MOST TRACE FUNCTION WERE FROM OTHER NS3 EXAMPLE                               */
/*ADDED TRACERS ARE DELTA AND REAL RTT AS WELL AS MODIFIED RttTracer FOR nodeID */
/*******************************************************************************/
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


/***FIXED SHARE ESTIMATE****************************************************************/
static void
FixedShareTracer (Time oldval, Time newval)
{
  if (firstFixed)
    {
      *fixedStream->GetStream () <<  "0.0 " << oldval.GetSeconds () << std::endl;
      firstFixed = false;
    }
  *fixedStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

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
/***FIXED SHARE ESTIMATE****************************************************************/
static void
TraceFixedShare (std::string fixed_tr_file_name)
{
  AsciiTraceHelper ascii;
  fixedStream = ascii.CreateFileStream (fixed_tr_file_name.c_str ());
  Config::ConnectWithoutContext (FixedSharePath, MakeCallback (&FixedShareTracer));
}

int
main (int argc, char *argv[])
{
  uint32_t run = 0;                 // RUN INDEXT FOR REPEATABLE SEED
  uint32_t seed = 1;                // SEED FOR REPEATABILITY
  uint32_t maxBytes = 300;          // MAX NUMBER OF BYTES TRANSMITTED PER NODE IN MEGABYTES
                                    // USED IN CONSTANT TRAFFIC AND INCREASING TRAFFIC MODEL


  double SimTime = 10.0;            // IN SECONDS
  double error_p = 0.0;             // FOR ADDING ERROR PROPABILITY
  double range = 250;               // RANGE FOR TRANSMISSION CUTOFF
  double delay_start_interval = 2.0;// DELAY INTERVAL FOR INCREASING TRAFFIC MODEL FOR ADDING FLOWS

  uint16_t num_add_trans_node = 3;  // NUMBER OF NODES/FLOWS TO BE ADDED PER INTERVAL FOR INCREASING TRAFFIC MODEL
  uint16_t num_flows = 1;           // NUMBER OF FLOWS IN THE SIMULATION

  std::string transport_prot = "TcpWestwood";   // TCP VARIANT
  std::string mobility_model = "RandomWalk";    // MOBILITY MODEL
  std::string placement = "Grid";               // NODE PLACEMENT MODEL
  std::string traffic_pattern = "Increasing";   // TRAFFIC PATTERN MODEL

  /*LOG COMPONENTS*/
  LogComponentEnable("WirelessRtt", LOG_LEVEL_INFO);
  //  LogComponentEnable("RttEstimator", LOG_LEVEL_INFO);
  //  LogComponentEnable("ConstantRateWifiManager", LOG_LEVEL_INFO);
  //  LogComponentEnable ("BulkSendApplication", LOG_LEVEL_INFO);
  //  LogComponentEnable ("TcpSocketBase", LOG_LEVEL_LOGIC);


  /*TRACE FILE NAMES*/
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
  std::string fixed_tr_file_name = "wireless.fixedEst";


  /*COMMAND PROMPT: DISPLAY FOR --help AND TAKES IN ARGS TO MODIFY PARAMETERS*/
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
  cmd.AddValue ("fixed_tr_name", "Name of output trace file for Fixed Share Estimate: ", fixed_tr_file_name);
  cmd.Parse (argc, argv);

  /*SET RUN SEEDS FOR REPEATABILITY*/
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




  /*CREATE NODES - WIFI AP AND STATIONS*/
  NS_LOG_INFO ("Create nodes.");
  NodeContainer stas, ap;
  ap.Create(1);
  NS_LOG_INFO ("Number of Nodes After AP: " << NodeList::GetNNodes());
  stas.Create(num_flows);


  /*DISIPLAY SIMULATION PARAMETERS*/
  NS_LOG_INFO ("Total Number of Nodes After Adding Wifi Stations: " << NodeList::GetNNodes());
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

  /*CONFIGURE PACKET ERROR RATE*/
  Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable> ();
  uv->SetStream (50);
  RateErrorModel error_model;
  error_model.SetRandomVariable (uv);
  error_model.SetUnit (RateErrorModel::ERROR_UNIT_PACKET);
  error_model.SetRate (error_p);



  /*CONFIGURE L1/2*/
  NS_LOG_INFO ("Create channels.");
  /* Wifi Channel*/
  YansWifiChannelHelper wifiChannelHelper = YansWifiChannelHelper::Default ();
  Ptr<YansWifiChannel> wifiChannel = wifiChannelHelper.Create ();

  /* Physical Layer*/
  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
  wifiPhyHelper.SetChannel(wifiChannel);


  /* Mac Layer*/
  NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default ();
  Ssid ssid = Ssid ("ns-3-ssid");

  /*PROPAGATION DELAY MODEL*/
  NS_LOG_INFO ("Propataion Delay Model: Random Propagation Delay Model");
  wifiChannelHelper.SetPropagationDelay ("ns3::RandomPropagationDelayModel");

  /*CUT OFF RANGE*/
  NS_LOG_INFO ("Propagation range: " << range);
  /*PROPAGATION LOSS MODEL*/
  wifiChannelHelper.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(range));
  //MORE MODELS AT https://www.nsnam.org/docs/release/3.15/doxygen/group__propagation.html
  NS_LOG_INFO ("===================================================================");


  /*INSTALL L1/L2 ON NODES*/
  /*INSTALL ON MOBILE STATION*/
  wifiMacHelper.SetType ("ns3::StaWifiMac",
                        "Ssid", SsidValue (ssid),
                        "ActiveProbing", BooleanValue (false));
  WifiHelper wifiHelper = WifiHelper::Default ();
  wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211a);
  wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate54Mbps"));
  NetDeviceContainer wifiContainer = wifiHelper.Install(wifiPhyHelper, wifiMacHelper, stas);


  /*INSTALL ON WIRELESS AP*/
  wifiMacHelper.SetType ("ns3::ApWifiMac",
                       "Ssid", SsidValue (ssid));
  wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211a);
  wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate54Mbps"));
  NetDeviceContainer apContainer = wifiHelper.Install(wifiPhyHelper, wifiMacHelper, ap);



  NS_LOG_INFO ("Install Internet Stack.");

  /*INSTALL INTERNET STACK*/
  InternetStackHelper internet;
  internet.Install (stas);
  internet.Install (ap);



  /*ASSIGN IP*/
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (apContainer);
  Ipv4InterfaceContainer i1 = ipv4.Assign (wifiContainer);



  /*CONFIGURE MOBILITY MODEL*/
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  /*LOCATION FOR STATIONARY AP X , Y, Z -AXIS*/
  positionAlloc->Add (Vector (50.0, 50.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  /*CONSTANT POSITION*/
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (ap);


  /*MOBILITY FOR ALL OTHER NODES*/
  if (placement.compare ("Grid") == 0)
  {
    /*GRID PLACEMENT, NODES ARE PLACE SPAVED OUT ON A GRID WITH FOLLOWING PARAMETERS*/
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (35.0),
                                  "MinY", DoubleValue (40.0),
                                  "DeltaX", DoubleValue (5.0),
                                  "DeltaY", DoubleValue (10.0),
                                  "GridWidth", UintegerValue (5),
                                  "LayoutType", StringValue ("RowFirst"));
 }else if (placement.compare ("UniformDisc") == 0)
 {
   /*UNIFORM DISK, NODES ARE SCATTERED UNIFORMLY IN THIS RADIUS AROUND THE SPECIFIED AXIS*/
   double radius = 20.0;
   mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
                                       "X", DoubleValue (50.0),
                                       "Y", DoubleValue (50.0),
                                       "rho", DoubleValue (radius));
  }
  /*OTHER PLACEMENT MODELS*/
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
    /*STATIONARY*/
    mobility.SetMobilityModel ("ns3::ConstantPosition");
  } else if (mobility_model.compare ("RandomWaypoint") == 0)
  {
    /*RANDOM WAY POINT*/
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

  /*INSTALL CONFIGURED MOBILITY TO MOBILE NODES*/
  mobility.Install (stas);





  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;  // well-known echo port number

  ApplicationContainer sourceApps;

  /*BURST MODE CONFIGURATIONS*/
  if (traffic_pattern.compare ("Burst") == 0)
  {
    NS_LOG_INFO ("Traffic Pattern: Burst" );
    // Create the OnOff applications to send TCP
    OnOffHelper source ("ns3::TcpSocketFactory", InetSocketAddress (i.GetAddress (0), port));
    source.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    //source.SetAttribute ("OffTime", RandomVariableValue (ConstantVariable (0)));
    source.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));

    /*INSTALL ON MOBILE STATION*/
    for (int i = 0; i < num_flows; i++)
      {
        sourceApps = source.Install (stas.Get (i));
        sourceApps.Start (Seconds (0.1));
        sourceApps.Stop (Seconds (SimTime));
      }

  } else
  {
     /*BULK SEND APPLICATION*/
      /*NAME OF PROTOCOL, DESTINATION ADDRESS*/
      BulkSendHelper source ("ns3::TcpSocketFactory",
                             InetSocketAddress (i.GetAddress (0), port));
      //source.SetAttribute("SendSize", UintegerValue (1500));
      // Set the amount of data to send in bytes.  Zero is unlimited.
      source.SetAttribute ("MaxBytes", UintegerValue (int(maxBytes* 1000000)));


    if (traffic_pattern.compare ("Constant") ==  0)
    {
      /*CONSTANT TRANSMISSION*/
      NS_LOG_INFO ("Traffic Pattern: Constant");
      for (int i = 0; i < num_flows; i++)
        {
          sourceApps = source.Install (stas.Get (i));
          sourceApps.Start (Seconds (0.1));
          sourceApps.Stop (Seconds (SimTime));
        }
    }
    else if (traffic_pattern.compare ("Increasing") ==  0)
    {
      /*INCREASING FLOWS OVERTIME*/
      NS_LOG_INFO ("Traffic Pattern: Increasing");
      int j = 0;
      for (int i = 0; i < num_flows; i++)
        {
          if (i % num_add_trans_node == 2)  j++ ;
          sourceApps = source.Install (stas.Get (i));
          sourceApps.Start (Seconds (0.0 + (j*delay_start_interval)));
          sourceApps.Stop (Seconds (SimTime));
        }
    }
  }


  /*INSTALL SINK APPLICATION ON STATIONARY AP*/
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (ap.Get (0));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (SimTime));

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1460));
//
// Set up tracing if enabled
//
  if (true)
    {
      //AsciiTraceHelper ascii;
      //wifiHelper.EnableAsciiAll (ascii.CreateFileStream ("wireless_rtt.tr"));
      wifiPhyHelper.EnablePcapAll ("wireless_rtt", true);
    }
//
// Now, do the actual simulation.
//
  // Setting up Animation Interface nicely

    AnimationInterface anim ("wireless_rtt.xml");
    /*Assign Label for access point*/
    anim.UpdateNodeDescription (ap.Get (0), "wireless access point");
    /*base station color*/
    anim.UpdateNodeColor(ap.Get (0), 0, 0,255);

    /*Assign Label for mobile wireless stations*/
    for (int i = 0; i < num_flows; i++)
      {
        char str[18];
        sprintf(str,"wireless station %d", (i+1));
        anim.UpdateNodeDescription (stas.Get (i), str);
        /*enable packet metadata*/
        anim.EnablePacketMetadata(anim_meta);
      }

  /*Residual code, keeps trace of total databytes recieved at sink*/
  Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));

/********************************************************************************/
/*HOOKS FOR STARTING TRACES *****************************************************/
/********************************************************************************/
double trace_start=0.5; //MUST BE SUFFICIENTLY LARGE ENOUGH FOR ALL SOCKETS TO BE CREATED TO TRACE CORRECTLY
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
  /*****Fixed Share*********************************************************/
if (delta_tr_file_name.compare ("") != 0)
  {
    Simulator::Schedule (Seconds (trace_start), &TraceFixedShare, fixed_tr_file_name);
  }

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (SimTime));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  //PRINTS OUT THROUGHPUT INFO OF FIRST SINK
  std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
  std::cout << "Total Throughput: " << (sink1->GetTotalRx ()*8)/(1000000*SimTime) << " Mbps" << std::endl;
}
