# C++ Coding Challenge: Build a Periodic Task Scheduler
​
## Why?
​
Monitoring internal and external systems requires a fixed sample rate in order to perform statistical analysis and correlate data from multiple sources.
​
## Requirements
​
Implemented a periodic task scheduler in modern C++.
​
- The scheduler supports two kinds of tasks: network and system probes. See below for the metrics you can collect on each kind of probe.
- It should be possible to customise the execution frequency for each task separately.
- Each task should return one or more “metrics” in the form of decimal values.
- Apart from raw metric data, the program also calculates some aggregate metrics. For example: average, minimum, and maximum. The aggregate metrics is kept up to date for each new data point that you collect.
- It is possible to stop and restart the scheduler and it will continue from where it left off.
​
Currently 1 task is implemented:
### Network probe Metrics
Choose from one of the following metrics for the Network Probe:
- Time to resolve a domain name to an IP address
- Time to connect to a TCP server, e.g., google.com port 80
- ICMP ping time to a server, e.g., google.com
​
​