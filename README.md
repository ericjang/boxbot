# BoxBot Simulator
==============

BoxBot is a C++ framework for embodied robot simulation in 2D environments.

<!-- TODO : handdrawn boxbot here -->

## Features
- The `Agent` class represents a robot that can interact with a simulated world. It owns a `Policy` object that takes in observations of the world state and returns controls that the Agent will apply.
- Physics simulation and agent embodiment handled via the LiquidFun simulator (the Agent is just a set of bodies and joints).
- LiquidFun environment and Agent can be defined via Protobuf and loaded into the simulator.
- `RPCPolicy` is a `Policy` subclass that computes controls via [gRPC](). Agent observations are serialized into a Protobuf and sent to a Policy Server. The policy server computes controls and sends them back to the agent. This allows us to implement policies in any environment that supports Google protobufs & gPRC : Python + TensorFlow, LuaJIT + Torch, etc.
- Python Interface for LiquidFun worlds
- Simulator can run in real-time with OpenGL debug visualization, or headless mode
- Cross platform: runs on Windows, Mac OSX, Linux

## Screenshots


## Installation

Prerequisites:
- Google Protobufs
- gRPC

```bash
git clone https://github.com/ericjang/boxbot
```

## Simulation File Format


```
$ ./boxbot octopus.proto
```
loads the world, the agent, and the policy and runs the simulation.


## License

Copyright (c) 2016, Eric Jang

Open-source under Apache 2.0
