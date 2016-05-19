# BoxBot Simulator
==============

BoxBot is a C++ framework for embodied robot physics simulation in 2D environments, built on top of the LiquidFun library.

![BoxBot screenshot 1](http://i.imgur.com/F1ujNPf.png)
![BoxBot screenshot 2](http://imgur.com/8LzOo7w.png)
![BoxBot screenshot 3](http://imgur.com/Fm4OepZ.png)

## Features
- The `Agent` class represents a robot that can interact with a simulated world. It owns a `Policy` object that takes in observations of the world state and returns controls that the Agent will apply.
- Physics simulation and agent embodiment handled via the LiquidFun simulator (the Agent is just a set of bodies and joints).
- `RPCPolicy` is a `Policy` subclass that computes controls via [gRPC](http://www.grpc.io/). Agent observations are serialized into a Protobuf and sent to a Policy Server. The policy server computes controls and sends them back to the agent. This allows us to implement policies in any environment that supports Google protobufs & gPRC : Python + TensorFlow, LuaJIT + Torch, etc.
- Python Interface for LiquidFun worlds
- Simulator can run in real-time with OpenGL debug visualization, or headless mode
- Cross platform: runs on Windows, Mac OSX, Linux

## Installation

Prerequisites:
- Google Protobufs
- gRPC

```bash
git clone https://github.com/ericjang/boxbot
qmake && make
```

## License

Copyright (c) 2016, Eric Jang

Open-source under Apache 2.0
