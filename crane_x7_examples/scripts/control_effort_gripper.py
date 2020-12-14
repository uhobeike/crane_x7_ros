#! /usr/bin/env python
# coding: utf-8

import actionlib
from control_msgs.msg import GripperCommandAction, GripperCommandGoal
import moveit_commander
import rospy


GRIPPER_OPEN = 1.0
GRIPPER_CLOSE = 0.0
def stop():
    arm.set_named_target("vertical")
    arm.go()

    gripper_goal.command.position = GRIPPER_OPEN
    gripper.send_goal(gripper_goal)

def main():
    arm.set_named_target("home")
    arm.go()

    r = rospy.Rate(60)
    start_time = rospy.Time.now().secs
    open_gripper = True
    while not rospy.is_shutdown():
        present_time = rospy.Time.now().secs
        if present_time - start_time >= 3.0:
            start_time = present_time
            open_gripper = not open_gripper

            if open_gripper:
                rospy.loginfo("Open")
                gripper_goal.command.position = GRIPPER_OPEN
                gripper_goal.command.max_effort = 1.0
                gripper.send_goal(gripper_goal)
            else:
                rospy.loginfo("Close")
                gripper_goal.command.position = GRIPPER_CLOSE
                gripper_goal.command.max_effort = 1.0
                gripper.send_goal(gripper_goal)

        r.sleep()


if __name__ == '__main__':
    rospy.init_node("control_effort_gripper")
    rospy.on_shutdown(stop)

    robot = moveit_commander.RobotCommander()
    arm = moveit_commander.MoveGroupCommander("arm")
    arm.set_max_velocity_scaling_factor(0.3)
    arm.set_max_acceleration_scaling_factor(1.0)

    gripper = actionlib.SimpleActionClient("crane_x7/gripper_controller/gripper_cmd", GripperCommandAction)
    gripper.wait_for_server()
    gripper_goal = GripperCommandGoal()

    try:
        if not rospy.is_shutdown():
            main()
    except rospy.ROSInterruptException:
        pass
