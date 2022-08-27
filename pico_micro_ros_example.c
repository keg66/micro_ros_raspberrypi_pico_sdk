#include <stdio.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/bool.h>

#include "pico/stdlib.h"
#include "pico_uart_transports.h"

#include "rustlib.h"

const uint LED_PIN = 25;

rcl_publisher_t publisher;  // publisher struct in rcl
std_msgs__msg__Int32 msg;

void timer_callback(rcl_timer_t* timer, int64_t last_call_time)
{
  rcl_ret_t ret = rcl_publish(&publisher, &msg, NULL);
  msg.data += rust_function();
}

void subscription_callback(const void* msgin)
{
  const std_msgs__msg__Bool* msg_ptr = (const std_msgs__msg__Bool*)msgin;
  gpio_put(LED_PIN, msg_ptr->data);
}

int main()
{
  rmw_uros_set_custom_transport(true,  // Framing enabled here. Using Stream-oriented mode.
                                NULL,  // void * args,
                                pico_serial_transport_open, pico_serial_transport_close, pico_serial_transport_write,
                                pico_serial_transport_read);

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);  // Set a single GPIO direction.

  rcl_timer_t timer;
  rcl_node_t node;
  rcl_allocator_t allocator;
  rclc_support_t support;
  rclc_executor_t executor;
  rcl_subscription_t subscriber;

  allocator = rcl_get_default_allocator();  // Return a properly initialized rcl_allocator_t with default values.

  // Wait for agent successful ping for 2 minutes.
  const int timeout_ms = 1000;
  const uint8_t attempts = 120;

  rcl_ret_t ret = rmw_uros_ping_agent(timeout_ms, attempts);  // Check if micro-ROS Agent is up and running.

  if (ret != RCL_RET_OK)
  {
    // Unreachable agent, exiting program.
    return ret;
  }

  rclc_support_init(&support, 0, NULL, &allocator);  // Initializes rcl and creates some support data structures.

  rclc_node_init_default(&node, "pico_node" /* node name */, "" /* namespace */,
                         &support);  // Creates a default RCL node.
  rclc_publisher_init_default(
      &publisher, &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs /* PkgName */, msg /* MsgSubfolder */, Int32 /* MsgName */),
      "pico_publisher" /* topic name */);  // Creates an rcl publisher.

  rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(1000) /* timeout_ns */,
                          timer_callback);  // Creates an rcl timer.

  rclc_subscription_init_default(&subscriber, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
                                 "std_msgs_msg_Bool");

  rclc_executor_init(&executor, &support.context, 2 /* number_of_handles */, &allocator);  // Initializes an executor.
  // number_of_handles: the total number of subscriptions, timers, services, clients and guard conditions.
  //                    Do not include the number of nodes and publishers.
  rclc_executor_add_timer(&executor, &timer);  // Adds a timer to an executor.
  rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA);

  gpio_put(LED_PIN, 1);

  msg.data = 0;
  while (true)
  {
    // The spin-some function checks one-time for new data from the DDS-queue.
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }
  return 0;
}
