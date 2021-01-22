/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 --------------|\                                       /|---------n3
// n1 --------------| > n_mux(mux)------------n_demux(demux)< |---------n4
// n2 --------------|/                                       \|---------n5
//    point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ATM_UNAM");

typedef struct struc{
  uint8_t flag;
  uint8_t add;
  uint8_t ctrl;
  uint16_t proto;
  uint8_t pay[1500];
} ppp;

typedef union u{
  unsigned char a[1505];
  ppp pp;
} my_union;

int contador = 0;
uint8_t m[3];
NetDeviceContainer devices_principal;

void PaqRecibidoEnMuxI1(Ptr<const Packet> p)
{
  u_int32_t size = p->GetSize ();
  uint8_t tmp[size];
  p->CopyData (tmp,size);
  m[0] = tmp[2];
  contador++;

  for( int i = 0 ; i < size ; i++ ){
    std::cout << (int)tmp[i] << "-";
  }

  std::cout << " -> " << p->GetSize ()<< std::endl;

  //Revisar si ya tengo los 3 paquetes
  if(contador == 3){
      Ptr<Packet> pkt = Create<Packet> (m,3);
      Address add = devices_principal.Get (1)->GetAddress ();
      devices_principal.Get (0)->Send (pkt,add,0x0800);
    }
  //Una vez que tenga los 3, los transmito por la interfaz principal
}

void PaqRecibidoEnMuxI2(Ptr<const Packet> p)
{
  u_int32_t size = p->GetSize ();
  uint8_t tmp[size];
  p->CopyData (tmp,size);
  m[1] = tmp[2];
  contador++;

  for( int i = 0 ; i < size ; i++ ){
    std::cout << (int)tmp[i] << "-";
  }

  std::cout << " -> " << p->GetSize ()<< std::endl;

  //Revisar si ya tengo los 3 paquetes
  if(contador == 3){
      Ptr<Packet> pkt = Create<Packet> (m,3);
      Address add = devices_principal.Get (1)->GetAddress ();
      devices_principal.Get (0)->Send (pkt,add,0x0800);
    }
  //Una vez que tenga los 3, los transmito por la interfaz principal
}

void PaqRecibidoEnMuxI3(Ptr<const Packet> p)
{
  u_int32_t size = p->GetSize ();
  uint8_t tmp[size];
  p->CopyData (tmp,size);
  m[2] = tmp[2];
  contador++;

  for( int i = 0 ; i < size ; i++ ){
    std::cout << (int)tmp[i] << "-";
  }

  std::cout << " -> " << p->GetSize ()<< std::endl;

  //Revisar si ya tengo los 3 paquetes
  if(contador == 3){
      Ptr<Packet> pkt = Create<Packet> (m,3);
      Address add = devices_principal.Get (1)->GetAddress ();
      devices_principal.Get (0)->Send (pkt,add,0x0800);
    }
  //Una vez que tenga los 3, los transmito por la interfaz principal
}

void PaqRecibidoEnDemux(Ptr<const Packet> p)
{
  uint8_t mensaje[5];
  p->CopyData (mensaje,5);

  char recibido[3];
  recibido[0] = mensaje[2];
  recibido[1] = mensaje[3];
  recibido[2] = mensaje[4];

  std::cout << "Demux recibe " << recibido << " -> " << p->GetSize ()<< std::endl;
  //Desempaquetar los 3 paquetitos y enviar cada uno a las interfaces secundarias
}

void PaqRecibidoEnNodo(Ptr<const Packet> p)
{
  std::cout << p << " -> " << p->GetSize ()<< std::endl;
  //Imprimir el contenido del paquete
}

int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodos_tx;
  nodos_tx.Create (3);
  NodeContainer nodo_mux;
  nodo_mux.Create (1);
  NodeContainer nodo_demux;
  nodo_demux.Create (1);
  NodeContainer nodos_rx;
  nodos_rx.Create (3);
  NodeContainer nodes;
  nodes.Add (nodos_tx);
  nodes.Add (nodo_mux);
  nodes.Add (nodo_demux);
  nodes.Add (nodos_rx);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("200kbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0ms"));

  NetDeviceContainer devices1;
  devices1 = pointToPoint.Install (nodos_tx.Get (0),nodo_mux.Get (0));
  NetDeviceContainer devices2;
  devices2 = pointToPoint.Install (nodos_tx.Get (1),nodo_mux.Get (0));
  NetDeviceContainer devices3;
  devices3 = pointToPoint.Install (nodos_tx.Get (2),nodo_mux.Get (0));

  //Faltan las interfaces de los enlaces de recepcion
  NetDeviceContainer devices4;
  devices4 = pointToPoint.Install (nodo_demux.Get (0),nodos_rx.Get (0));
  pointToPoint.EnablePcapAll ("tdm");
  //...

  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("1200kbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("0ms"));

  devices_principal = pointToPoint2.Install (nodo_mux.Get (0),nodo_demux.Get (0));

  NetDeviceContainer devices;

  devices.Add (devices1);
  devices.Add (devices2);
  devices.Add (devices3);
  devices.Add (devices_principal);

  //Nodo 3
  uint8_t m3[1];
  m3[0] = '6';
  Ptr<Packet> pkt3 = Create<Packet> (m3,1);
  Address add3 = devices3.Get (1)->GetAddress ();
  devices3.Get (0)->Send (pkt3,add3,0x0800);
  //Nodo 1
  uint8_t m1[1];
  m1[0] = 'A';
  Ptr<Packet> pkt = Create<Packet> (m1,1);
  Address add = devices1.Get (1)->GetAddress ();
  devices1.Get (0)->Send (pkt,add,0x0800);
  //Nodo 2
  my_union m2;
  //m2.i = 500;
  Ptr<Packet> pkt2 = Create<Packet> (m2.a,15);
  Address add2 = devices2.Get (1)->GetAddress ();
  devices2.Get (0)->Send (pkt2,add2,0x0800);
  //Crear paquetes para los otros nodos y ponerles datos

  //Activar las Trace Sources de todos los nodos
  devices1.Get (1)->TraceConnectWithoutContext ("PhyRxEnd", MakeCallback(&PaqRecibidoEnMuxI1));
  devices2.Get (1)->TraceConnectWithoutContext ("PhyRxEnd", MakeCallback(&PaqRecibidoEnMuxI2));
  devices3.Get (1)->TraceConnectWithoutContext ("PhyRxEnd", MakeCallback(&PaqRecibidoEnMuxI3));
  devices_principal.Get(1)->TraceConnectWithoutContext ("PhyRxEnd", MakeCallback(&PaqRecibidoEnDemux));
  devices4.Get (1)->TraceConnectWithoutContext ("PhyRxEnd", MakeCallback(&PaqRecibidoEnNodo));
  //...

  Simulator::Run ();
  Simulator::Destroy ();

/*
  my_union u;
  u.i = 2147483647;
  std::cout << (int)u.a[0] << "." << (int)u.a[1] << "." << (int)u.a[2] << "." << (int)u.a[3];*/


  return 0;
}
