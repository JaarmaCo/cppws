# Suggested API for process based request managers

If each request will be managed by each process, we can easily abstract the endpoint manager as the
program entry point of the request manager.

If we are to continue with the idea of modelling the framework after the Java Rest framework, we can
do something "_reflective-esc_" for specifying these endpoints.

The user would provide a function that serves as the endpoint manager. This function would accept an
arbitrary number of arguments, where the arguments are processed by the framework and injected through
an automatic mechanism.

Given that C++ is a naturally _static_ language, it makes sense to implement this through either templates
or some pre-compilation tooling.

## Suggested usage

```c++
#include <format>
#include <iostream>

#include <nlohmann/json.hpp>

#include <cppws/endpoint.hpp>

namespace http = cppws::http;

WS_BINDING("userdb")
extern cppws::database database;

WS_BINDING("tgenerator")
extern cppws::token_generator tgen;

void WS_POST("/api/users/login") login(const nlohmann::json &args) {

  auto password = database.build_query("SELECT password FROM users WHERE username={}", args["username"])
    .find_one();

  if (cppws::cryptographic_hash(args["password"]) != password) {
    std::cout << http::ACCESS_DENIED << http::body("Invalid password");
    return;
  }

  std::cout << http::OK << http::body(
    std::format("Token: {}", tgen.generate(args["username"])));
}
```

## Dealing with scalability

One problem with the process based approach is that it is typically slower when it comes to sharing resources
and communicating between requests/the master process. Another major problem is that `fork` is a relatively
expensive operation.

### Pooling

The solution to this problem is to, like the threaded method, pool request handler processes in a slave pool
managed by a master process (that owns the server socket). This removes the overhead of forking new processes at
every request, and allows for the server to optimize itself after the parallelism potential on the local machine.

### Clustering

One major benefit of having an inter-process based request management is that it allows for requests to be dispatched
to other machines than the one owning the original connection. This basically removes the resource problem, as you are
now bounded mainly by the distribution time of the message between the master process and the slaves on the external
machines.

The main problem is that you are now dealing with a distributed system, which is by far a lot more complex than local
only systems. Another problem is that the overhead of dispatching the request becomes a lot higher because you now need
to wait for the request to distribute across the cluster. However, the scalability is excellent, and with sufficiently
fast network speed within the cluster, it can grow almost arbitrarily large (and handle a large number of requests).

Having the requests be clustered also improves isolation by _a lot_ as you are now not only not sharing memory between
requests, you are no longer even sharing the same hardware. An attacker could potentially infiltrate and entirely take
over one node in the cluster, and it would not affect the rest of the network.

Also, clustered networks could potentially be more tolerant to DDOS attacks, as a clustered network is a lot more load
tolerant. A clustered network could simply offload the work of accepting requests to other machines if one becomes
overloaded, making it so that even if one accepting node crashes, others could come and accept the requests for it.

## The clustered architecture

Given the previous example of DDOS attack protection, it makes sense for the architecture to be able to spawn both worker,
and receiver processes. To manage all of this, there are two potential solutions to ensure proper work offload and resource
management.

### Overseer - civilized architecture

One is to assign an overseer process that handles dispatching workers and receivers. The overseer would be one (or more in case
of large clusters) process in the cluster center. This process has priority and privilege over the workers and receivers, and
would:

- Own the original server socket
- Have the sole privilege to spawn new processes
- Have the sole privilege to kill processes
- Can terminate the entire cluster

The strengths of this solution is that it isolates the weak points of the cluster. The core overseer process is the main target
of infiltration (as the workers and receivers can all be highly containerized), meaning that we can put a lot of security measures
on this particular process.

This is also the downside of this method. As there is one node that effectively _owns_ the rest of the cluster, if that node where
to be compromised, it would immediately put the entire network at risk.

### Distributed authority - organic architecture

Another method is to distribute process spawning and termination across all nodes in the network. One node could _itself_ detect
if it is overloaded, and spawn a detached child process. The main benefit here is that it allows for faster response times, as
processes do not need to connect to an overseer before taking recovery actions.

It would work something like this:

- A process spawns a detached child when overloaded.
- Processes regularly communicate with each other to ensure integrity.
- Jury instead of judge - a node majority can collectively kill a malicious process.
- Shutdown through propagation - shutdown signals are propagated node to node.

The main problem here is that the distributed authority is individually weaker than the overseer. It is a lot harder to secure
the cluster if every node in the network is a potential attack surface. The main benefit however is that even if these nodes where
to be compromised, as long as a majority of the nodes are not, that node is detected and terminated.

#### Process cancer

One peculiar observation that can be made about this type of system is that it has the potential to develop "_cancer-like behaviour_".
An attacker could potentially trick a process into replicating itself without limit. Once that process clones itself past the number
of benevolent processes, the "_organism_" (network) dies.

One method to combat this would be to come up with some method of detecting when this type of behaviour occurs. Once process cancer
is detected, processes may release "_antibodies_" that will target the signature of the cancerous process and attempt to destroy it by
majority.

### Coolness factor

Fuck all other methods, the biological method is far to cool to not to attempt. The base idea is to let each single unit behave like
a biological cell with some sort of specialization. What we have is essentially a "living" organism. You must also include the potential
that you could potentially slap on an _AI_ sticker on the project, which could improve exposure.

The major structure would be like this:

- Molecules - code segments that perform some action without conscientious decision making.
  - ATP - atom of CPU time and/or memory.
  - DNA - base code for constructing new cells (basically the user defined code of the server).
  - Immune indicators - marker for malicious cells, marker for intact cells (checksum, etc.) and
  other immune functions.
  - Hormones - messages sent cell-to-cell that modifies local behaviour.
  - Toxins - molecules that when detected, cause cells to immediately die (shutdown mechanism).
- Cells - perform actions in their local environment based on simple rules (typically one process).
  - Cell death - cells willfully terminate themselves if they notice system overloads or when no
  longer required.
  - Mitosis - cells spawn more cells.
  - Metabolism - cells _consume_ CPU/memory.
  - Stem cells - pool of idle cells waiting for quick dispatch into burdened parts of the network.
- Tissues/organs - clusters of cells that cooperate on performing one task.
  - Neurons/brain - makes decisions for the network at large.
  - Immune system - identifies and terminates malicious processes.
  - Circulatory system - distributes information across the network.
