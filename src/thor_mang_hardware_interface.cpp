#include <thor_mang_ros_control/thor_mang_hardware_interface.h>

namespace Thor
{
const std::string ThorMangHardwareInterface::jointUIDs[38-1] =
{
  "r_shoulder_pitch",
  "l_shoulder_pitch",
  "r_shoulder_roll",
  "l_shoulder_roll",
  "r_shoulder_yaw",
  "l_shoulder_yaw",
  "r_elbow",
  "l_elbow",
  "r_wrist_yaw1",
  "l_wrist_yaw1",
  "r_wrist_roll",
  "l_wrist_roll",
  "r_wrist_yaw2",
  "l_wrist_yaw2",
  "r_hip_yaw",
  "l_hip_yaw",
  "r_hip_roll",
  "l_hip_roll",
  "r_hip_pitch",
  "l_hip_pitch",
  "r_knee",
  "l_knee",
  "r_ankle_pitch",
  "l_ankle_pitch",
  "r_ankle_roll",
  "l_ankle_roll",
  "waist_pan",
  "waist_tilt",
  "head_pan",
  "head_tilt",
  "r_f0_j0",        // r_hand_thumb
  "l_f0_j0",        // l_hand_thumb
  "r_f1_j0",        // r_hand_index_finger
  "l_f1_j0",        // l_hand_index_finger
  "r_hand_middle_finger",
  "l_hand_middle_finger",
  "waist_lidar"
};

const std::string ThorMangHardwareInterface::ftSensorUIDs[ThorMangHardwareInterface::MAXIMUM_NUMBER_OF_FT_SENSORS] =
{
  "r_hand",
  "l_hand",
  "r_foot",
  "l_foot"
};

const int ThorMangHardwareInterface::ros_joint_offsets[38-1] =
{
  0,        // r_shoulder_pitch
  0,        // l_shoulder_pitch
  -125500,  // r_shoulder_roll
  125500,   // l_shoulder_roll
  125500,   // r_shoulder_yaw
  -125500,  // l_shoulder_yaw
  -62750,   // r_elbow
  62750,    // l_elbow
  0,        // r_wrist_yaw1
  0,        // l_wrist_yaw1
  0,        // r_wrist_roll
  0,        // l_wrist_roll
  0,        // r_wrist_yaw2
  0,        // l_wrist_yaw2
  0,        // r_hip_yaw
  0,        // l_hip_yaw
  0,        // r_hip_roll
  0,        // l_hip_roll
  0,        // r_hip_pitch
  0,        // l_hip_pitch
  62750,    // r_knee
  -62750,   // l_knee
  0,        // r_ankle_pitch
  0,        // l_ankle_pitch
  0,        // r_ankle_roll
  0,        // l_ankle_roll
  0,        // waist_pan
  0,        // waist_tilt
  0,        // head_pan
  0,        // head_tilt
  1600,     //r_f0_j0	// r_hand_thumb
  1330,     //l_f0_j0	// l_hand_thumb
  1530,     //r_f1_j0	// r_hand_index_finger
  1200,     //l_f1_j0	// l_hand_index_finger
  0,        // r_hand_middle_finger
  0,        // l_hand_middle_finger
  0         // waist_lidar
};

ThorMangHardwareInterface::Ptr ThorMangHardwareInterface::singelton = ThorMangHardwareInterface::Ptr();

ThorMangHardwareInterface::ThorMangHardwareInterface()
  : hardware_interface::RobotHW()
  , joint_state_intervall(20.0)
  , last_joint_state_read(ros::Time::now())
{
 // uID = const_cast<char*>("thor_mang_hardware_interface");
}

ThorMangHardwareInterface::ThorMangHardwareInterface(ThorMangHardwareInterface const&)
{
}

ThorMangHardwareInterface::~ThorMangHardwareInterface()
{
}

ThorMangHardwareInterface& ThorMangHardwareInterface::operator=(ThorMangHardwareInterface const&)
{
}

ThorMangHardwareInterface::Ptr& ThorMangHardwareInterface::Instance()
{
  if (!singelton)
    singelton.reset(new ThorMangHardwareInterface());
  return singelton;
}

void ThorMangHardwareInterface::Initialize()
{
  // INS
  initINS();

//  if (MotionStatus::m_CurrentJoints.size() != 0)
//  {
//    m_RobotInfo = MotionStatus::m_CurrentJoints;

//    // start robot hardware
//    robotBringUp();
//  }
//  else
//  {
//    ROS_WARN("MotionStatus is not initialized");
//  }

  /** register joints */

  // dispatching joints
//  for (unsigned int joint_index = 0; joint_index < m_RobotInfo.size(); joint_index++)
//  {
//    if (m_RobotInfo[joint_index].m_ID > 38-1)
//    {
//      ROS_WARN("Robot has joint with invalid id: %u", m_RobotInfo[joint_index].m_ID);
//      continue;
//    }

//    unsigned int id_index = m_RobotInfo[joint_index].m_ID-1;

//    // connect and register the joint state interface
//    hardware_interface::JointStateHandle joint_state_handle(jointUIDs[id_index], &pos[id_index], &vel[id_index], &eff[id_index]);
//    joint_state_interface.registerHandle(joint_state_handle);

//    // connect and register the joint position interface
//    hardware_interface::JointHandle joint_handle(joint_state_handle, &cmd[id_index]);
//    pos_joint_interface.registerHandle(joint_handle);

//    // activate control
//    MotionStatus::m_EnableList[id_index].uID = uID;
//  }

  registerInterface(&joint_state_interface);
  registerInterface(&pos_joint_interface);

  /** register sensors */
  // IMU
  imu_data.name = "pelvis_imu";
  imu_data.frame_id = "pelvis";
  imu_data.orientation = imu_orientation;
  imu_data.angular_velocity = imu_angular_velocity;
  imu_data.linear_acceleration = imu_linear_acceleration;
  hardware_interface::ImuSensorHandle imu_sensor_handle(imu_data);
  imu_sensor_interface.registerHandle(imu_sensor_handle);
  registerInterface(&imu_sensor_interface);

  // FT-Sensors
  for (unsigned int sensorIndex = 0; sensorIndex < MAXIMUM_NUMBER_OF_FT_SENSORS; sensorIndex++)
  {
    hardware_interface::ForceTorqueSensorHandle force_torque_sensor_handle_raw(ftSensorUIDs[sensorIndex] + "_raw", ftSensorUIDs[sensorIndex], force_raw[sensorIndex], torque_raw[sensorIndex]);
    force_torque_sensor_interface.registerHandle(force_torque_sensor_handle_raw);

    hardware_interface::ForceTorqueSensorHandle force_torque_sensor_handle_compensated(ftSensorUIDs[sensorIndex], ftSensorUIDs[sensorIndex], force_compensated[sensorIndex], torque_compensated[sensorIndex]);
    force_torque_sensor_interface.registerHandle(force_torque_sensor_handle_compensated);
  }
  registerInterface(&force_torque_sensor_interface);

  // footstep interface
  hardware_interface::ThorMangFootstepsHandle footsteps_handle("footsteps_handle", &getDynamixelMutex());
  footstep_interface.registerHandle(footsteps_handle);
  registerInterface(&footstep_interface);

  // load compensation data from parameter server
  for (unsigned int sensorIndex = 0; sensorIndex < MAXIMUM_NUMBER_OF_FT_SENSORS; sensorIndex++) {
      ros::NodeHandle nh(ftSensorUIDs[sensorIndex]);
      if (!(ft_compensation[sensorIndex].loadMassComBias(nh) &&
            ft_compensation[sensorIndex].loadHandToSensorOffset(nh, "sensor_offset"))) {
          ROS_WARN_STREAM("Couldn't load complete ft sensor compensation data for " << ftSensorUIDs[sensorIndex] << " in " << nh.getNamespace() << ".");
      }
  }

  robot_transforms.init();
}

void ThorMangHardwareInterface::Process()
{
  //  boost::mutex::scoped_lock lock(hardware_mutex);
  //  m_RobotInfo[m_Pan_joint_index].m_Value = m_RobotInfo[m_Pan_joint_index].m_DXLInfo->Rad2Value(cmd[m_Pan_joint_index]);
  //  m_RobotInfo[m_Tilt_joint_index].m_Value = m_RobotInfo[m_Tilt_joint_index].m_DXLInfo->Rad2Value(cmd[m_Tilt_joint_index]);
  //  m_RobotInfo[m_Tilt_joint_index].m_Pgain = 8;
}

void ThorMangHardwareInterface::read(ros::Time time, ros::Duration period)
{
  for (unsigned int i = 0; i < 38-1; i++)
    cmd[i] = std::numeric_limits<double>::quiet_NaN();

//  // read joint values
//  for (unsigned int joint_index = 0; joint_index < MotionStatus::m_CurrentJointsStatus.size(); joint_index++)
//  {
//    int id_index = MotionStatus::m_CurrentJointsStatus[joint_index].m_ID-1;
//    pos[id_index] = MotionStatus::m_CurrentJointsStatus[joint_index].m_DXLInfo->Value2Rad(MotionStatus::m_CurrentJointsStatus[joint_index].m_Value - ros_joint_offsets[id_index]);
//  }

//  // Update Robot state
//  for (unsigned int i = 0; i < 30; i++) // iterate over all body joints
//    robot_transforms.updateState(jointUIDs[i], pos[i]);

//  // read IMU and transform it to pelvis frame
//  tf::Quaternion imu_orient;
//  imu_orient.setRPY(ins->GetEulerAngle().pitch, ins->GetEulerAngle().roll, -ins->GetEulerAngle().yaw);

//  imu_orientation[0] = imu_orient.x();
//  imu_orientation[1] = imu_orient.y();
//  imu_orientation[2] = imu_orient.z();
//  imu_orientation[3] = imu_orient.w();

//  imu_angular_velocity[0] =  ins->GetGyroData().scaled_gyro[1];
//  imu_angular_velocity[1] =  ins->GetGyroData().scaled_gyro[0];
//  imu_angular_velocity[2] = -ins->GetGyroData().scaled_gyro[2];

//  imu_linear_acceleration[0] =  ins->GetAccelData().scaled_accel[1] * G_ACC;
//  imu_linear_acceleration[1] =  ins->GetAccelData().scaled_accel[0] * G_ACC;
//  imu_linear_acceleration[2] = -ins->GetAccelData().scaled_accel[2] * G_ACC;

//  // Update robot state root transform
//  Eigen::Affine3d imu_orient_rot(Eigen::Quaternion<double>(imu_orientation[3], imu_orientation[0], imu_orientation[1], imu_orientation[2]));
//  imu_orient_rot.translation()  = Eigen::Vector3d::Zero();
//  robot_transforms.updateRootTransform(imu_orient_rot);

  // read FT-Sensors
//  force_raw[R_ARM][0] = MotionStatus::R_ARM_FX;
//  force_raw[R_ARM][1] = -MotionStatus::R_ARM_FY;
//  force_raw[R_ARM][2] = MotionStatus::R_ARM_FZ;
//  torque_raw[R_ARM][0] = MotionStatus::R_ARM_TX;
//  torque_raw[R_ARM][1] = MotionStatus::R_ARM_TY;
//  torque_raw[R_ARM][2] = MotionStatus::R_ARM_TZ;

//  force_raw[L_ARM][0] = MotionStatus::L_ARM_FX;
//  force_raw[L_ARM][1] = MotionStatus::L_ARM_FY;
//  force_raw[L_ARM][2] = MotionStatus::L_ARM_FZ;
//  torque_raw[L_ARM][0] = MotionStatus::L_ARM_TX;
//  torque_raw[L_ARM][1] = MotionStatus::L_ARM_TY;
//  torque_raw[L_ARM][2] = MotionStatus::L_ARM_TZ;

//  force_raw[R_LEG][0] = MotionStatus::R_LEG_FX;
//  force_raw[R_LEG][1] = MotionStatus::R_LEG_FY;
//  force_raw[R_LEG][2] = MotionStatus::R_LEG_FZ;
//  torque_raw[R_LEG][0] = MotionStatus::R_LEG_TX;
//  torque_raw[R_LEG][1] = MotionStatus::R_LEG_TY;
//  torque_raw[R_LEG][2] = MotionStatus::R_LEG_TZ;

//  force_raw[L_LEG][0] = MotionStatus::L_LEG_FX;
//  force_raw[L_LEG][1] = MotionStatus::L_LEG_FY;
//  force_raw[L_LEG][2] = MotionStatus::L_LEG_FZ;
//  torque_raw[L_LEG][0] = MotionStatus::L_LEG_TX;
//  torque_raw[L_LEG][1] = MotionStatus::L_LEG_TY;
//  torque_raw[L_LEG][2] = MotionStatus::L_LEG_TZ;

  // apply compensation
  update_force_torque_compensation();
  update_force_torque_sensors();
}

void ThorMangHardwareInterface::write(ros::Time time, ros::Duration period)
{
//  if (m_RobotInfo.size() == 0)
//    return;

//  boost::mutex::scoped_lock lock(getDynamixelMutex());

//  for (unsigned int joint_index = 0; joint_index < m_RobotInfo.size(); joint_index++)
//  {
//    int id_index = m_RobotInfo[joint_index].m_ID-1;

//    if (cmd[id_index] == std::numeric_limits<double>::quiet_NaN())
//      continue;

//    if (m_RobotInfo[joint_index].m_ID < 1 || m_RobotInfo[joint_index].m_ID > 38-1)
//      continue;

//    m_RobotInfo[joint_index].m_Value = m_RobotInfo[joint_index].m_DXLInfo->Rad2Value(cmd[id_index]) + ros_joint_offsets[id_index];

//    // workaround for MX28 due to framework bug
//    if (m_RobotInfo[joint_index].m_DXLInfo->MODEL_NUM == 28)
//    {
//      int error;
//      MotionManager::GetInstance()->WriteWord(m_RobotInfo[joint_index].m_ID, MX28::P_GOAL_POSITION_L, m_RobotInfo[joint_index].m_Value, &error);
//    }
//  }
}

boost::mutex& ThorMangHardwareInterface::getDynamixelMutex()
{
  return Instance()->dynamixel_mutex;
}

void ThorMangHardwareInterface::setJointStateRate(double joint_state_rate)
{
  this->joint_state_intervall = 1.0/joint_state_rate;
}

void ThorMangHardwareInterface::setTorqueOn(int id, bool enable)
{
//  JointData* joint;
//  if (joint = getJoint(id))
//    setTorqueOn(*joint, enable);
//  else
//    ROS_ERROR("[setTorqueOn] No joint with ID %i available!", id);
}

void ThorMangHardwareInterface::setTorqueOn(bool enable)
{
  if (enable)
    ROS_INFO("Enable torque!");
  else
    ROS_INFO("Disable torque!");

//  for (unsigned int joint_index = 0; joint_index < m_RobotInfo.size(); joint_index++)
//  {
//    // ignore lidar motor
//    if (!enable && m_RobotInfo[joint_index].m_ID == 37)
//      continue;

//    setTorqueOn(m_RobotInfo[joint_index], enable);
//  }
}

bool ThorMangHardwareInterface::robotBringUp()
{
//  ROS_INFO("Setting up Dynamixal Pro...");
//  for (unsigned int joint_index = 0; joint_index < m_RobotInfo.size(); joint_index++)
//  {
//    if (m_RobotInfo[joint_index].m_DXLInfo->MODEL_NUM == 28)
//      continue;

//    int error = setIndirectAddress(joint_index);

//    if (error && error != 7)
//      ROS_ERROR("Error %d occured on ID %d", error, m_RobotInfo[joint_index].m_ID);

//    usleep(1000);
//  }

//  ROS_INFO("Moving to initial pose...");
//  if (!goReadyPose())
//    return false;

//  // init preview control module now
//  RecursiveWalking::GetInstance()->Initialize();
//  PreviewControlWalking::GetInstance()->Initialize();

//  ROS_INFO("Get initial foot FT sensor values...");
//  InitForceTorque();

//  ROS_INFO("Robot setup finished! You can place the robot on ground now.");
//  return true;
}

bool ThorMangHardwareInterface::goReadyPose()
{
  setTorqueOn(true);

//  // speed down servos
//  for (unsigned int joint_index = 0; joint_index < m_RobotInfo.size(); joint_index++)
//  {
//    int id = m_RobotInfo[joint_index].m_ID;

//    int error;
//    m_RobotInfo[joint_index].m_DXL_Comm->GetDXLInstance()->WriteDWord(id, PRO54::P_GOAL_ACCELATION_LL, 4, &error);
//    m_RobotInfo[joint_index].m_DXL_Comm->GetDXLInstance()->WriteDWord(id, PRO54::P_GOAL_VELOCITY_LL, 2000, &error);

//    if (error)
//      ROS_ERROR("Error %d occured on ID %d", error, id);

//    usleep(1000);
//  }

//  // compute trajectory
//  ROS_WARN("Compute trajectory to initial pose.");

//  int dir_output[16];
//  double InitAngle[16];

//  //for thor
//  dir_output[0] = -1; dir_output[1] = -1; dir_output[2] = -1; dir_output[3] = -1; dir_output[4] =  1; dir_output[5] = 1;
//  dir_output[6] = -1; dir_output[7] = -1; dir_output[8] =  1; dir_output[9] =  1; dir_output[10]= -1; dir_output[11] = 1;
//  dir_output[12] = -1; dir_output[13] = 1;  dir_output[14] = -1; dir_output[15] =  1;
//  InitAngle[0]  =   0.0;  InitAngle[1]  =  0.0;  InitAngle[2]  =  5.7106;  InitAngle[3] =  33.5788; InitAngle[4]  = -5.7106; InitAngle[5]  = 0.0;
//  InitAngle[6]  =   0.0;  InitAngle[7]  =  0.0;  InitAngle[8]  = -5.7106;  InitAngle[9] = -33.5788; InitAngle[10] =  5.7106; InitAngle[11] = 0.0;
//  InitAngle[12] = -45.0,  InitAngle[13] = 45.0;  InitAngle[14] =  45.0;    InitAngle[15] =  -45.0;

//  double angle[16];
//  int outValue[16];

//  matd GtoCOB = GetTransformMatrix(0, 0, 650.0, 0, 0, 0 );
//  matd GtoRF = GetTransformMatrix(0, -125.0, 0, 0, 0, 0);
//  matd GtoLF = GetTransformMatrix(0,  125.0, 0, 0, 0, 0);

//  matd RHtoCOB = GetTranslationMatrix(0,  Kinematics::LEG_SIDE_OFFSET*0.5, 0);
//  matd LHtoCOB = GetTranslationMatrix(0, -Kinematics::LEG_SIDE_OFFSET*0.5, 0);

//  matd COBtoG = GetTransformMatrixInverse(GtoCOB);
//  matd RHtoRF = RHtoCOB*COBtoG*GtoRF;
//  matd LHtoLF = LHtoCOB*COBtoG*GtoLF;

//  Pose3D epr, epl;

//  epr = GetPose3DfromTransformMatrix(RHtoRF);
//  epl = GetPose3DfromTransformMatrix(LHtoLF);

//  if (PreviewControlWalking::GetInstance()->computeIK(&angle[0], epr.x, epr.y, epr.z+Kinematics::LEG_LENGTH, epr.roll, epr.pitch, epr.yaw) == false)
//  {
//    ROS_ERROR("IKsolve failed");
//    return false;
//  }

//  if (PreviewControlWalking::GetInstance()->computeIK(&angle[6], epl.x, epl.y, epl.z+Kinematics::LEG_LENGTH, epl.roll, epl.pitch, epl.yaw) == false)
//  {
//    ROS_ERROR("IKsolve failed");
//    return false;
//  }

//  for (int idx = 0; idx < 6; idx++)
//  {
//    angle[idx] = (double)dir_output[idx]*angle[idx]*180.0/M_PI + InitAngle[idx];
//    angle[idx+6] = (double)dir_output[idx+6]*angle[idx+6]*180.0/M_PI + InitAngle[idx+6];
//  }


//  for (int idx = 0; idx < 16; idx++)
//  {
//    outValue[idx] = 251000.0*(angle[idx])/180.0;
//  }

//  double gdHipPitchOffset = 7.0;
//  outValue[2] -= (double)dir_output[2] * gdHipPitchOffset * 251000.0/180.0;
//  outValue[8] -= (double)dir_output[8] * gdHipPitchOffset * 251000.0/180.0;

//  for (unsigned int joint_index = 0; joint_index < m_RobotInfo.size(); joint_index++)
//  {
//    int id = m_RobotInfo[joint_index].m_ID;

//    int error;
//    if (id >= 15 && id <= 26)
//      m_RobotInfo[joint_index].m_DXL_Comm->GetDXLInstance()->WriteWord(id, PRO54::P_VELOCITY_I_GAIN_L, 0, &error);

//    if (error)
//      ROS_ERROR("Error %d occured on ID %d", error, id);

//    usleep(1000);
//  }

//  // let's move now
//  ROS_WARN("Moving to ready pose now!");

//  for (unsigned int joint_index = 0; joint_index < m_RobotInfo.size(); joint_index++)
//  {
//    int id = m_RobotInfo[joint_index].m_ID;

//    if (id == 1)
//      initJointPosition(joint_index, -62750);
//    else if (id == 2)
//      initJointPosition(joint_index, 62750);
//    else if (id == 3)
//      initJointPosition(joint_index, -109520);
//    else if (id == 4)
//      initJointPosition(joint_index, 109520);
//    else if (id == 5)
//      initJointPosition(joint_index, 125500);
//    else if (id == 6)
//      initJointPosition(joint_index, -125500);
//    else if (id == 7)
//      initJointPosition(joint_index, 62750);
//    else if (id == 8)
//      initJointPosition(joint_index, -62750);
//    else if (id == 9)
//      initJointPosition(joint_index, -75000);
//    else if (id == 10)
//      initJointPosition(joint_index,  75000);


//    else if ((id == 27) || (id == 28) || (id == 29))
//      initJointPosition(joint_index,  0);
//    else if (id == 30)
//      initJointPosition(joint_index,  0);
//    else if (id == 11)
//      initJointPosition(joint_index,  0);
//    else if (id == 12)
//      initJointPosition(joint_index,  0);
//    else if (id == 13)
//      initJointPosition(joint_index,  0);
//    else if (id == 14)
//      initJointPosition(joint_index,  0);


//    else if (id == 15)
//      initJointPosition(joint_index,  outValue[0] + MotionManager::GetInstance()->m_Offset[14]);
//    else if (id == 17)
//      initJointPosition(joint_index,  outValue[1] + MotionManager::GetInstance()->m_Offset[16]);
//    else if (id == 19)
//      initJointPosition(joint_index,  outValue[2] + MotionManager::GetInstance()->m_Offset[18]);
//    else if (id == 21)
//      initJointPosition(joint_index,  outValue[3] + MotionManager::GetInstance()->m_Offset[20]);
//    else if (id == 23)
//      initJointPosition(joint_index,  outValue[4] + MotionManager::GetInstance()->m_Offset[22]);
//    else if (id == 25)
//      initJointPosition(joint_index,  outValue[5] + MotionManager::GetInstance()->m_Offset[24]);

//    else if (id == 16)
//      initJointPosition(joint_index,  outValue[6] + MotionManager::GetInstance()->m_Offset[15]);
//    else if (id == 18)
//      initJointPosition(joint_index,  outValue[7] + MotionManager::GetInstance()->m_Offset[17]);
//    else if (id == 20)
//      initJointPosition(joint_index,  outValue[8] + MotionManager::GetInstance()->m_Offset[19]);
//    else if (id == 22)
//      initJointPosition(joint_index,  outValue[9] + MotionManager::GetInstance()->m_Offset[21]);
//    else if (id == 24)
//      initJointPosition(joint_index,  outValue[10] + MotionManager::GetInstance()->m_Offset[23]);
//    else if (id == 26)
//      initJointPosition(joint_index,  outValue[11] + MotionManager::GetInstance()->m_Offset[25]);

//    usleep(1000);
//  }

//  usleep(5000000);
//  for (unsigned int index = 0; index < m_RobotInfo.size(); index++) {
//    int id = m_RobotInfo[index].m_ID;

//    int error;
//    m_RobotInfo[index].m_DXL_Comm->GetDXLInstance()->WriteDWord(id, PRO54::P_GOAL_ACCELATION_LL, 0, &error);
//    m_RobotInfo[index].m_DXL_Comm->GetDXLInstance()->WriteDWord(id, PRO54::P_GOAL_VELOCITY_LL, 0, &error);

//    if (error)
//      ROS_ERROR("Error %d occured on ID %d", error, id);

//    usleep(1000);
//  }

//  return true;
//}

//int ThorMangHardwareInterface::setIndirectAddress(unsigned int joint_index)
//{
//  int id = m_RobotInfo[joint_index].m_ID;
//  DXLComm* comm = m_RobotInfo[joint_index].m_DXL_Comm;
//  int error = 0;

//  //0
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_0_L, PRO54::P_PRESENT_POSITION_LL, &error);
//  if (error)
//    return error;

//  //1
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_1_L, PRO54::P_PRESENT_POSITION_LH, &error);
//  if (error)
//    return error;

//  //2
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_2_L, PRO54::P_PRESENT_POSITION_HL, &error);
//  if (error)
//    return error;

//  //3
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_3_L, PRO54::P_PRESENT_POSITION_HH, &error);
//  if (error)
//    return error;

//  //4
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_4_L, PRO54::P_EXTERNAL_PORT_DATA_1_L, &error);
//  if (error)
//    return error;

//  //5
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_5_L, PRO54::P_EXTERNAL_PORT_DATA_1_H, &error);
//  if (error)
//    return error;

//  //6
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_6_L, PRO54::P_EXTERNAL_PORT_DATA_2_L, &error);
//  if (error)
//    return error;

//  //7
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_7_L, PRO54::P_EXTERNAL_PORT_DATA_2_H, &error);
//  if (error)
//    return error;

//  //8
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_8_L, PRO54::P_EXTERNAL_PORT_DATA_3_L, &error);
//  if (error)
//    return error;

//  //9
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_9_L, PRO54::P_EXTERNAL_PORT_DATA_3_H, &error);
//  if (error)
//    return error;

//  //10
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_10_L, PRO54::P_EXTERNAL_PORT_DATA_4_L, &error);
//  if (error)
//    return error;

//  //11
//  comm->GetDXLInstance()->WriteWord(id, PRO54::P_INDIRECT_ADDRESS_11_L, PRO54::P_EXTERNAL_PORT_DATA_4_H, &error);
//  if (error)
//    return error;

  return 0;
}

void ThorMangHardwareInterface::initJointPosition(unsigned int joint_index, int value)
{
//  int error;
//  m_RobotInfo[joint_index].m_DXL_Comm->GetDXLInstance()->WriteDWord(m_RobotInfo[joint_index].m_ID, PRO54::P_GOAL_POSITION_LL, value, &error);
//  MotionStatus::m_CurrentJoints[joint_index].m_Value = m_RobotInfo[joint_index].m_Value = value;

//  if (error)
//    ROS_ERROR("Error %d occured on ID %d", error, m_RobotInfo[joint_index].m_ID);
}

void ThorMangHardwareInterface::initINS()
{
  imu_orientation[0] = imu_orientation[1] = imu_orientation[2] = 0.0;
  imu_orientation[3] = 1.0;
  imu_angular_velocity[0] = imu_angular_velocity[1] = imu_angular_velocity[2] = 0.0;
  imu_linear_acceleration[0] = imu_linear_acceleration[1] = imu_linear_acceleration[2] = 0.0;

  // in case of reinit
//  if (ins)
//  {
//    ROS_INFO("Reinitialize INS!\n");
//    ins->StopSensingDataUpdate();
//    ins->Disconnect();
//  }

//  // init INS
//  ins.reset(new Ins());
//  if (ins)
//  {
//    if (ins->Connect("ttyACM0", 921600) != MIP_INTERFACE_OK)
//      ROS_ERROR("Failed to connect INS!\n");
//    else if (ins->Initialize() != MIP_INTERFACE_OK)
//      ROS_ERROR("Failed to init INS!\n");
//    else if (ins->SetEnableAHRSDataCallBack() != MIP_INTERFACE_OK)
//      ROS_ERROR("Failed to init AHRS callback!\n");
//    else
//      ins->StartSensingDataUpdate();
//  }
}

void ThorMangHardwareInterface::InitForceTorque()
{
  force_raw[R_ARM][0] = force_raw[R_ARM][1] = force_raw[R_ARM][2] = 0.0;
  torque_raw[R_ARM][0] = torque_raw[R_ARM][1] = torque_raw[R_ARM][2] = 0.0;

  force_raw[L_ARM][0] = force_raw[L_ARM][1] = force_raw[L_ARM][2] = 0.0;
  torque_raw[L_ARM][0] = torque_raw[L_ARM][1] = torque_raw[L_ARM][2] = 0.0;

  force_raw[R_LEG][0] = force_raw[R_LEG][1] = force_raw[R_LEG][2] = 0.0;
  torque_raw[R_LEG][0] = torque_raw[R_LEG][1] = torque_raw[R_LEG][2] = 0.0;

  force_raw[L_LEG][0] = force_raw[L_LEG][1] = force_raw[L_LEG][2] = 0.0;
  torque_raw[L_LEG][0] = torque_raw[L_LEG][1] = torque_raw[L_LEG][2] = 0.0;

  double Init_right_fx_N = 0,  Init_right_fy_N = 0,  Init_right_fz_N = 0;
  double Init_right_Tx_Nm = 0, Init_right_Ty_Nm = 0, Init_right_Tz_Nm = 0;
  double Init_left_fx_N = 0,  Init_left_fy_N = 0,  Init_left_fz_N = 0;
  double Init_left_Tx_Nm = 0, Init_left_Ty_Nm = 0, Init_left_Tz_Nm = 0;
  int InitAngleWindowSize = 125;
  for (int count = 0; count < InitAngleWindowSize; count++)
  {
//    Init_right_fx_N  += MotionStatus::R_LEG_FX;
//    Init_right_fy_N  += MotionStatus::R_LEG_FY;
//    Init_right_fz_N  += MotionStatus::R_LEG_FZ;
//    Init_right_Tx_Nm += MotionStatus::R_LEG_TX;
//    Init_right_Ty_Nm += MotionStatus::R_LEG_TY;
//    Init_right_Tz_Nm += MotionStatus::R_LEG_TZ;

//    Init_left_fx_N  += MotionStatus::L_LEG_FX;
//    Init_left_fy_N  += MotionStatus::L_LEG_FY;
//    Init_left_fz_N  += MotionStatus::L_LEG_FZ;
//    Init_left_Tx_Nm += MotionStatus::L_LEG_TX;
//    Init_left_Ty_Nm += MotionStatus::L_LEG_TY;
//    Init_left_Tz_Nm += MotionStatus::L_LEG_TZ;

//    // trigger process and withit sensor readings
//    boost::mutex::scoped_lock lock(getDynamixelMutex());
//    MotionManager::GetInstance()->Process();
//    usleep(MotionModule::TIME_UNIT * 1000.0);
  }

  Init_right_fx_N = Init_right_fx_N/(double)InitAngleWindowSize;
  Init_right_fy_N = Init_right_fy_N/(double)InitAngleWindowSize;
  Init_right_fz_N = Init_right_fz_N/(double)InitAngleWindowSize;
  Init_right_Tx_Nm = Init_right_Tx_Nm/(double)InitAngleWindowSize;
  Init_right_Ty_Nm = Init_right_Ty_Nm/(double)InitAngleWindowSize;
  Init_right_Tz_Nm = Init_right_Tz_Nm/(double)InitAngleWindowSize;
  Init_left_fx_N = Init_left_fx_N/(double)InitAngleWindowSize;
  Init_left_fy_N = Init_left_fy_N/(double)InitAngleWindowSize;
  Init_left_fz_N = Init_left_fz_N/(double)InitAngleWindowSize;
  Init_left_Tx_Nm = Init_left_Tx_Nm/(double)InitAngleWindowSize;
  Init_left_Ty_Nm = Init_left_Ty_Nm/(double)InitAngleWindowSize;
  Init_left_Tz_Nm = Init_left_Tz_Nm/(double)InitAngleWindowSize;

//  RecursiveWalking::GetInstance()->SetInitForceTorque(Init_right_fx_N ,  Init_right_fy_N ,  Init_right_fz_N ,
//                                                      Init_right_Tx_Nm , Init_right_Ty_Nm , Init_right_Tz_Nm,
//                                                      Init_left_fx_N ,  Init_left_fy_N,  Init_left_fz_N,
//                                                      Init_left_Tx_Nm , Init_left_Ty_Nm, Init_left_Tz_Nm);

//  PreviewControlWalking::GetInstance()->SetInitForceTorque(Init_right_fx_N ,  Init_right_fy_N ,  Init_right_fz_N ,
//                                                           Init_right_Tx_Nm , Init_right_Ty_Nm , Init_right_Tz_Nm,
//                                                           Init_left_fx_N ,  Init_left_fy_N,  Init_left_fz_N,
//                                                           Init_left_Tx_Nm , Init_left_Ty_Nm, Init_left_Tz_Nm);
}

void ThorMangHardwareInterface::update_force_torque_compensation()
{
  for (unsigned int i = 0; i < MAXIMUM_NUMBER_OF_FT_SENSORS; i++)
  {
    Eigen::Matrix3d world_gripper_rot = (robot_transforms.getRootTransform().rotation() * robot_transforms.getTransform(ftSensorUIDs[i]).rotation()).inverse();
    ft_compensation[i].setWorldGripperRotation(world_gripper_rot);
  }
}

void ThorMangHardwareInterface::update_force_torque_sensors()
{
  compensate_force_torque(R_ARM);
  compensate_force_torque(L_ARM);
  compensate_force_torque(R_LEG);
  compensate_force_torque(L_LEG);
}

void ThorMangHardwareInterface::compensate_force_torque(int ft_sensor_index)
{
  FTCompensation::Vector6d ft_raw;
  FTCompensation::Vector6d ft_compensated;

  ft_raw[0] = force_raw[ft_sensor_index][0];
  ft_raw[1] = force_raw[ft_sensor_index][1];
  ft_raw[2] = force_raw[ft_sensor_index][2];
  ft_raw[3] = torque_raw[ft_sensor_index][0];
  ft_raw[4] = torque_raw[ft_sensor_index][1];
  ft_raw[5] = torque_raw[ft_sensor_index][2];

  ft_compensation[ft_sensor_index].zeroAndCompensate(ft_raw, ft_compensated);

  force_compensated[ft_sensor_index][0] = ft_compensated[0];
  force_compensated[ft_sensor_index][1] = ft_compensated[1];
  force_compensated[ft_sensor_index][2] = ft_compensated[2];
  torque_compensated[ft_sensor_index][0] = ft_compensated[3];
  torque_compensated[ft_sensor_index][1] = ft_compensated[4];
  torque_compensated[ft_sensor_index][2] = ft_compensated[5];
}
}
