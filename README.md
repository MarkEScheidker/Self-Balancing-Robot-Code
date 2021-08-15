# Self-Balancing-Robot-Code

## What is it?
The code for my first attempt at building a self-balancing robot.

## What can it do?
Currently, it's a full control loop that reads data from an IMU, interprets that data using a complimentary filter deriving an angle, and uses that angle to send appropriately timed pulses to a pair of DC motors.  

## Did it work?
The robot I built was a partial success. I 3d printed a frame for the batteries, motors, raspberry pi, and other components and wired it all up. It ended up having a high center of gravity which wasn't very good for balancing. On top of that, the motors I bought were geared down 48/1 hobby motors which meant that their max speed was still too slow, and the rotational momentum from the gearbox increased the time it took for the robot to react quickly. It ended up shaking back and forth no matter what I did, but it didn't fall over.

## What's next?
I don't want to keep editing this code, because the motors I chose seem to be a dead end, so i'm going to try something else. I want to try using stepper motors to make the next iteration, but that comes with its own challenges. This will be a separate project due to the differences between driving dc vs driving stepper motors.

## Can I use this code for my own project?
It's highly specific for my hardware, and a lot of changes will need to be made if anybody wants to use it, but if you find anything in here you want to use i'm happy to let you use it.

## If I want to use this, how do I get it to work?
With the current implementation, this will only work on a raspberry pi. You will also need to install the wiringPi library in order to compile this. Compile it with "g++ name.cpp -o name -lwiringPi -lm".

