# Network-App-Stress-Tester
Network App Stress Tester is a tool that is used to simulate heavy load on a network application to evaluate its performance, stability and responsiveness under various conditions.
You can use this code by compiling it using a C++ compiler and then running the resulting executable with two command line arguments: the hostname and port of the server you want to stress test.
For example, if you have compiled the code into an executable named network-stress-tester:

./network-stress-tester example.com 80

This will run the stress test against example.com on port 80. The output will be the average response time and average response length of all the requests made by the threads.
