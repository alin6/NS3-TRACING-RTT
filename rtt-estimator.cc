/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
//
// Copyright (c) 2006 Georgia Tech Research Corporation
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: Rajib Bhattacharjea<raj.b@gatech.edu>
//

// Ported from:
// Georgia Tech Network Simulator - Round Trip Time Estimator Class
// George F. Riley.  Georgia Tech, Spring 2002

// Base class allows variations of round trip time estimators to be
// implemented

#include <iostream>
#include <fstream>
#include <cmath>

#include "rtt-estimator.h"
#include "ns3/double.h"
#include "ns3/log.h"

namespace ns3 {



NS_LOG_COMPONENT_DEFINE ("RttEstimator");

NS_OBJECT_ENSURE_REGISTERED (RttEstimator);

/// Tolerance used to check reciprocal of two numbers.
static const double TOLERANCE = 1e-6;

TypeId
RttEstimator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RttEstimator")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
    .AddAttribute ("InitialEstimation",
                   "Initial RTT estimate",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&RttEstimator::m_initialEstimatedRtt),
                   MakeTimeChecker ())
  ;
  return tid;
}

Time
RttEstimator::GetEstimate (void) const
{
  return m_estimatedRtt;
}

Time
RttEstimator::GetVariation (void) const
{
  return m_estimatedVariation;
}

// Base class methods
RttEstimator::RttEstimator ()
  : m_nSamples (0)
{
  NS_LOG_FUNCTION (this);

  // We need attributes initialized here, not later, so use the
  // ConstructSelf() technique documented in the manual
  ObjectBase::ConstructSelf (AttributeConstructionList ());
  m_estimatedRtt = m_initialEstimatedRtt;
  m_estimatedVariation = Time (0);
  NS_LOG_DEBUG ("Initialize m_estimatedRtt to " << m_estimatedRtt.GetSeconds () << " sec.");
}

RttEstimator::RttEstimator (const RttEstimator& c)
  : Object (c),
    m_initialEstimatedRtt (c.m_initialEstimatedRtt),
    m_estimatedRtt (c.m_estimatedRtt),
    m_estimatedVariation (c.m_estimatedVariation),
    m_nSamples (c.m_nSamples)
{
  NS_LOG_FUNCTION (this);
}

RttEstimator::~RttEstimator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
RttEstimator::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void RttEstimator::Reset ()
{
  NS_LOG_FUNCTION (this);
  // Reset to initial state
  m_estimatedRtt = m_initialEstimatedRtt;
  m_estimatedVariation = Time (0);
  m_nSamples = 0;
}

uint32_t
RttEstimator::GetNSamples (void) const
{

  return m_nSamples;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Mean-Deviation Estimator

NS_OBJECT_ENSURE_REGISTERED (RttMeanDeviation);

TypeId
RttMeanDeviation::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RttMeanDeviation")
    .SetParent<RttEstimator> ()
    .SetGroupName ("Internet")
    .AddConstructor<RttMeanDeviation> ()
    .AddAttribute ("Alpha",
                   "Gain used in estimating the RTT, must be 0 <= alpha <= 1",
                   DoubleValue (0.125),
                   MakeDoubleAccessor (&RttMeanDeviation::m_alpha),
                   MakeDoubleChecker<double> (0, 1))
    .AddAttribute ("Beta",
                   "Gain used in estimating the RTT variation, must be 0 <= beta <= 1",
                   DoubleValue (0.25),
                   MakeDoubleAccessor (&RttMeanDeviation::m_beta),
                   MakeDoubleChecker<double> (0, 1))
  ;
  return tid;
}

RttMeanDeviation::RttMeanDeviation()
{
  NS_LOG_FUNCTION (this);
}

RttMeanDeviation::RttMeanDeviation (const RttMeanDeviation& c)
  : RttEstimator (c), m_alpha (c.m_alpha), m_beta (c.m_beta)
{
  NS_LOG_FUNCTION (this);
}

TypeId
RttMeanDeviation::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
RttMeanDeviation::CheckForReciprocalPowerOfTwo (double val) const
{
  NS_LOG_FUNCTION (this << val);
  if (val < TOLERANCE)
    {
      return 0;
    }
  // supports 1/32, 1/16, 1/8, 1/4, 1/2
  if (std::abs (1/val - 8) < TOLERANCE)
    {
      return 3;
    }
  if (std::abs (1/val - 4) < TOLERANCE)
    {
      return 2;
    }
  if (std::abs (1/val - 32) < TOLERANCE)
    {
      return 5;
    }
  if (std::abs (1/val - 16) < TOLERANCE)
    {
      return 4;
    }
  if (std::abs (1/val - 2) < TOLERANCE)
    {
      return 1;
    }
  return 0;
}

void
RttMeanDeviation::FloatingPointUpdate (Time m)
{
  NS_LOG_FUNCTION (this << m);

  // EWMA formulas are implemented as suggested in
  // Jacobson/Karels paper appendix A.2

  // SRTT <- (1 - alpha) * SRTT + alpha *  R'
  Time err (m - m_estimatedRtt);
  double gErr = err.ToDouble (Time::S) * m_alpha;
  m_estimatedRtt += Time::FromDouble (gErr, Time::S);

  // RTTVAR <- (1 - beta) * RTTVAR + beta * |SRTT - R'|
  Time difference = Abs (err) - m_estimatedVariation;
  m_estimatedVariation += Time::FromDouble (difference.ToDouble (Time::S) * m_beta, Time::S);




  /*************************/
  //input experts here?

/*******************************/


  return;
}

void
RttMeanDeviation::IntegerUpdate (Time m, uint32_t rttShift, uint32_t variationShift)
{
  NS_LOG_FUNCTION (this << m << rttShift << variationShift);
  // Jacobson/Karels paper appendix A.2
  int64_t meas = m.GetInteger ();

  /*USED TO RETURN REAL RTT*/
  m_SampledRTT = Time::From(meas) ;

  int64_t delta = meas - m_estimatedRtt.GetInteger ();

 /*USED TO RETURN CURRENT DELTA*/
  m_CurrentDelta = Time::From(delta);

  int64_t srtt = (m_estimatedRtt.GetInteger () << rttShift) + delta;
  m_estimatedRtt = Time::From (srtt >> rttShift);
  if (delta < 0)
    {
      delta = -delta;
    }
  delta -= m_estimatedVariation.GetInteger ();
  int64_t rttvar = m_estimatedVariation.GetInteger () << variationShift;
  rttvar += delta;
  m_estimatedVariation = Time::From (rttvar >> variationShift);


  return;
}

void
RttMeanDeviation::Measurement (Time m)
{
  NS_LOG_FUNCTION (this << m);
  if (m_nSamples)
    {
      // If both alpha and beta are reciprocal powers of two, updating can
      // be done with integer arithmetic according to Jacobson/Karels paper.
      // If not, since class Time only supports integer multiplication,
      // must convert Time to floating point and back again
      uint32_t rttShift = CheckForReciprocalPowerOfTwo (m_alpha);
      uint32_t variationShift = CheckForReciprocalPowerOfTwo (m_beta);
      if (rttShift && variationShift)
        {
          IntegerUpdate (m, rttShift, variationShift);
        }
      else
        {
          FloatingPointUpdate (m);
        }
#ifdef USE_FIXED_SHARE
        NS_LOG_INFO ("IN GET MEASUREMENT" );
        int64_t meas = m.GetInteger ();

          /*****************************************************************************/
          /* FIXED SHARE EXPERT CALCULATIONS                                           */
          /*****************************************************************************/
          /*Fixed-share: Calculating Loss Function*************/
            //input experts here?
            for (int k = 0; k < num_Experts; k++){
              if (meas <= expertPrediction[k])
                m_loss[k] = pow((expertPrediction[k]-meas),m_expert_penalty); //loss is calculated here
              else
                m_loss[k] = m_expert_penalty *  meas;
              if (k==99);
                //NS_LOG_INFO ("Fixed-share: Calculating Loss Function");
            }
            /*Fixed-share: Calculating Weights****************************************/
            for (int k = 0; k < num_Experts; k++){
              m_weights[k] = m_weights[k]*exp(-m_ETA*m_loss[k]);
            }
            for (int k = 0; k < num_Experts; k++){
              m_pool = (m_share *  m_weights[k])+ m_pool;
            }
            for (int k = 0; k < num_Experts; k++){
              m_weights[k] = ((1- m_share)* m_weights[k])+ (m_pool-(m_share *  m_weights[k]))/(num_Experts-1);
            }
            /******************************************/
            /* CALC TOTAL WEIGHT                      */
            /* AND TOTAL OF WEIGHTED PREDICTIONS      */
            /*****************************************/
            //RESET TO CALCULATE VALUES FOR NEXT ROUND?
            m_sum_prediction=0;
            m_sum_weight=0;
            m_pool=0;
            for (int k =0; k < num_Experts; k++){
              m_sum_prediction += (m_weights[k]*expertPrediction[k]);
              m_sum_weight += m_weights[k];
            }
            /*USE THE SUMS FOR PREDICTION*/
            m_prediction = Time::From ( m_sum_prediction / m_sum_weight );

            //Weight Normalization
            for (int k = 0; k < num_Experts; k++){
              m_weights[k]= m_weights[k]/m_sum_weight;
            }
            /*****************************************************************************/
            /*END FIXED SHARE EXPERT CALCULATIONS                                        */
            /*****************************************************************************/
#endif

    }
  else
    { // First sample
      m_estimatedRtt = m;               // Set estimate to current
      m_estimatedVariation = m / 2;  // And variation to current / 2
      NS_LOG_DEBUG ("(first sample) m_estimatedVariation += " << m);

      /*INITIALIZE ALPHAS FOR SENSE ********
      m_sense_alpha[0] = 0.1;
      m_sense_alpha[1] = 0.2;
      m_sense_alpha[2] = 0.3;
      m_sense_alpha[3] = 0.4;
      m_sense_alpha[4] = 0.5;
      m_sense_alpha[5] = 0.6;
      m_sense_alpha[6] = 0.7;
      m_sense_alpha[7] = 0.8;
      m_sense_alpha[8] = 0.9;
      ******************************/
#ifdef USE_FIXED_SHARE
      /*****************************************************************************/
      /* FOR FIXED SHARE EXPERT CALCULATIONS                                        */
      /*****************************************************************************/
      /**FIXED EXPERT INITIALIZATION AT INITIAL RTT SAMPLE****************************/
      m_sum_weight=0;
      m_sum_prediction=0;
      m_pool=0;
      //THESE RANGE VARS MUST BE MANUALLY CONFIGURED PER SIMULATION SET!
      m_range_min = 0.00625 ;   // min of expected rtt
      m_range_max = 1.30234;   // max of expected rtt
      std::fill_n(m_weights, 100, 0.01);
      std::fill_n(m_loss, 100, 0);

      m_expert_penalty = 2;
      m_ETA = 2; //LEARNING RATE
      m_share = 0.08;


      num_Experts = 100;
      for (int number = 0; number < num_Experts; number++){
        /*EVEN DISTRIBUTION BETWEEN ALL EXPERTS*/
        expertPrediction[number] = m_range_min + number*((2*(m_range_max-m_range_min))/num_Experts);
      }

      NS_LOG_INFO ("EXPERT #25 INITIAL PREDICTION VALUE: " << expertPrediction[25] );
      /*****************************************************************************/
      /* END OF FIXED SHARE EXPERT CALCULATIONS                                    */
      /*****************************************************************************/
#endif

    }
  m_nSamples++;
}

Ptr<RttEstimator>
RttMeanDeviation::Copy () const
{
  NS_LOG_FUNCTION (this);
  return CopyObject<RttMeanDeviation> (this);
}

void
RttMeanDeviation::Reset ()
{
  NS_LOG_FUNCTION (this);
  RttEstimator::Reset ();
}

Time
RttEstimator::MeasuredRttSample(void) const
{
  return m_SampledRTT;
}

Time
RttEstimator::CurrentDelta(void) const
{
  return m_CurrentDelta;
}

Time
RttEstimator::FixedShareEstimate(void) const
{
  return m_prediction;
}


} //namespace ns3
