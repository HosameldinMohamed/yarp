/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ODOMETRY2D_NWS_YARP_H
#define YARP_ODOMETRY2D_NWS_YARP_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IOdometry2D.h>
#include "Odometry2DServerImpl.h"
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/RpcServer.h>

#define DEFAULT_THREAD_PERIOD 0.02 //s

/**
 * @ingroup dev_impl_nws_yarp dev_impl_navigation
 *
 * \section Odometry2D_nws_yarp_parameters Device description
 * \brief `Odometry2D_nws_yarp`: A yarp nws to get the odometry and publish it on 3 yarp ports:
 *   - a port for odometry;
 *   - a port for odometer;
 *   - a port for velocity.
 * The attached device must implement a `yarp::dev::Nav2D::IOdometry2D` interface.
 *
 * Parameters required by this device are:
 * | Parameter name      | SubParameter            | Type    | Units          | Default Value         | Required                       | Description                                           | Notes |
 * |:-------------------:|:-----------------------:|:-------:|:--------------:|:---------------------:|:-----------------------------: |:-----------------------------------------------------:|:-----:|
 * | period              |      -                  | double  | s              | 0.02                  | No                             | refresh period of the broadcasted values in s         | default 0.02s |
 * | name                |      -                  | string  | -              | /odometry2D_nws_yarp  | No                             | The name of the server, used as a prefix for the opened ports   |      |
 * | subdevice           |      -                  | string  | -              |   -                   | alternative to 'attach' action | name of the subdevice to use as a data source         | when used, parameters for the subdevice must be provided as well |
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device odometry2D_nws_yarp
 * period 0.02
 * name /odometry2D_nws_yarp
 *
 * subdevice fakeOdometry
 * \endcode
 *
 * example of xml file with a fake odometer
 *
 * \code{.unparsed}
 * <?xml version="1.0" encoding="UTF-8" ?>
 * <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 3.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd">
 * <robot name="fakeOdometry" build="2" portprefix="test" xmlns:xi="http://www.w3.org/2001/XInclude">
 *   <devices>
 *     <device xmlns:xi="http://www.w3.org/2001/XInclude" name="fakeOdometry_device" type="fakeOdometry">
 *     </device>
 *     <device xmlns:xi="http://www.w3.org/2001/XInclude" name="odometry_nws_yarp" type="odometry2D_nws_yarp">
 *       <param name="name"> /odometry2D_nws_yarp </param>
 *       <action phase="startup" level="5" type="attach">
 *           <paramlist name="networks">
 *               <elem name="subdevice_odometry"> fakeOdometry_device </elem>
 *           </paramlist>
 *       </action>
 *       <action phase="shutdown" level="5" type="detach" />
 *     </device>
 *   </devices>
 * </robot>
 * \endcode
 *
 * example of command via terminal.
 *
 * \code{.unparsed}
 * yarpdev --device odometry2D_nws_yarp --period 0.01 --subdevice fakeOdometry
 * \endcode
 */

class Odometry2D_nws_yarp :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader
{
public:
    Odometry2D_nws_yarp();
    ~Odometry2D_nws_yarp();

    // DeviceDriver
    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    // WrapperSingle
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;

    // PeriodicThread
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    //thrift
    IOdometry2DRPCd  m_RPC;

    //rpc port
    bool read(yarp::os::ConnectionReader& connection) override;

    //buffered ports
    yarp::os::BufferedPort<yarp::dev::OdometryData> m_port_odometry;
    yarp::os::BufferedPort<yarp::os::Bottle> m_port_velocity;
    yarp::os::BufferedPort<yarp::os::Bottle> m_port_odometer;
    yarp::os::RpcServer                      m_rpcPort;

    //yarp streaming data
    std::string m_local_name = "/odometry2D_nws_yarp";
    std::string m_odometerStreamingPortName;
    std::string m_odometryStreamingPortName;
    std::string m_velocityStreamingPortName;
    std::string m_rpcPortName;
    std::string m_deviceName;
    size_t m_stampCount{0};
    yarp::dev::OdometryData m_oldOdometryData{0,0,0,0,0,0,0,0,0};

    // thread
    double m_period{DEFAULT_THREAD_PERIOD};

    // timestamp
    yarp::os::Stamp m_lastStateStamp;

    //interfaces
    yarp::dev::PolyDriver m_driver;
    yarp::dev::Nav2D::IOdometry2D *m_odometry2D_interface{nullptr};

};

#endif // YARP_ODOMETRY2D_NWS_YARP_H
