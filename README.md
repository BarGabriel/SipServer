# Sip Server

A simple sip server for handling VoIP calls based on sip protocol.

# Features
### Registration Of Users
The server supports registration process.
When a user send REGISTER request, the server replies with 200 OK response.
Note that the server does not validate user's credentials.

### Call Flow
The server supports a normal call flow.
When a user sends an INVITE request, the request is handled by the server and it is forward to the destination.
If the destination is busy, the server send SIP/2.0 486 Busy Here message to the source.
If the destination pick up the call, the server transfer 200 OK message to the source.
If the source want to cancel the call, the server send cancel message to the destination, and replies to the source with 200 OK message and SIP/2.0 487 Request Terminated message.
If one of the sides has been hung up, the server will got bye message which will be sending to the other side to indicate the end of the call.


# Build

#### Windows 
Using Visual Studio command prompt

```bash
  mkdir build && cd build
  cmake ..
  msbuild SipServer.sln
```

#### Linux 

```bash
  mkdir build && cd build
  cmake ..
  make
```
# Program options
`--ip=` The sip server ip.  
`--port=` The sip server port. The default value is 5060.  

# Usage Guide
1. Download a softphone software like [Zoiper](https://www.zoiper.com/en/voip-softphone/download/current), [Express Talk](https://www.nch.com.au/talk/index.html) or any other software.
2. Create new sip accounts and set their domain to the sip server ip.
3. Run the sip server.
4. Register the accounts and make calls :)
