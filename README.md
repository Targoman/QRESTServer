[![Build Status](https://travis-ci.org/Targoman/QRESTServer.svg?branch=master)](https://travis-ci.org/Targoman/QRESTServer)

# QRESTServer

 QRESTServer focuses on developing a lean and mean Qt/C++ based API server.
 In addition to leveraging modern C++, QRESTServer also employs Qt to promote productivity and reinforce best practices.
 QRESTServer allows you to create auto generated RESTfull APIs along with a Websocket server.
 Controllable Caching mechanisms - Both internal and centralized (based on Redis) - implemented in QRESTServer allows you create high performance APIs.

 ### Table of contents
- [Motivation](#motivation)
- [Sample codes](#sample-codes)
- [Features](#features)
- [Setup](#setup)
- [Source tree](#source-tree)
- [Disclaimer](#disclaimer)
- [License](#license)


 # Motivation
 [TOC](#table-of-contents)

 Nowadays most of the APIs are written in high-level languages such as PHP, python, GO, JS, ruby, etc.
 While using these high-level languages causes rapid development it will decrease performance.
 Fig.1 shows a performance comparison between common RESTfull API Servers

 As can be seen in Fig.1 vanilla PHP is too slow and can be ignored and other frameworks are some near in results.
 Fig.2 show a detailed comparison ignoring PHP as a choice.


 Apart from performance issue which can be ignored in most of cases, in complex tasks where there are multiple
 inter-communiactions betwen low-level/high-level componenets are required to respond an API call
 (ex. inter-communicaiton betwwen micro-services), using such langugaes make things some complex.
 In Targoman Translation Framework, on each call to translation API we need to first normalize/tokenize input text,
 then classify and choose appropiate translation engine, next call the chosen translation engine and get back results
 and detokenize final translation. This process requires inter-communication between at least 4 independent microservices,
 while high-level logic also depends on the result of intermediate micro-services results. Using QRESTServer we could
 embed some of these micro-services (as libraries) in the main API and avoid multiple intermediate API calls.

 You may ask:
 * why C++? A short answer is performance and easy integration to our other developed libraries.
 * But why Qt?

 # Sample codes
[TOC](#table-of-contents)

 # Features
[TOC](#table-of-contents)

# Setup
[TOC](#table-of-contents)


# Source Tree
 [TOC](#table-of-contents)


# License
[TOC](#table-of-contents)

QRESTServer has been published under the terms of [GNU AFFERO GENERAL PUBLIC LICENSE](./LICENSE) as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


