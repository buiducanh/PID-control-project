# PID Controller Reflection
## Describe the effect each of the P, I, D components had in your implementation.

- P: this component reacts to the cross track error directly so that the steering will adjust the heading back to the reference trajectory. If this component, the coefficient, is large, then the car will make dramatic turn towards the center of the road. Tuning this component helps the car make sharp turns on the track. However, using this component alone, or when this component becomes dominant, the car swerves a lot up and down the center and frequently goes off the track.

- I: this component helps the car stay right in the center, since this component reacts to the sum of the cross track error from the start. Without this component, the car cannot fix the bias to the cross track error of the initial position.

- D: this component reacts to the change in cross track error between two consecutive frames. The component effectively smoothes out all the other control parameters and help the car smoothly steer towards a lower cross track error. The cumulative effect is that this component helps the car not swerve as much and smoothly make the turns.

## Describe how the final hyperparameters were chosen.

I implemented a version of twiddle (Twiddle.cpp) as mentioned in the course. The main difference is that my version needs to run online with the autonomous run in the simulator. Hence, I implement an internal counter in the Twiddle class so that each 10 frames of the simulator, Twiddle will tune the parameters accordingly, this is hyper-localized to very small sections of the track but the automation helps my manual tuning tremendously. However, I found running Twiddle alone does not get the car out of the ditch and most of the times cause the car to get stuck or loop around.

Thus, I use twiddle on each of the 3 components alone and get a reasonable initial values for the components (so that the car will not get stuck) and then use twiddle outputs to run twiddle again tuning all 3 at the same time. Eventually I found a combination that worked.

