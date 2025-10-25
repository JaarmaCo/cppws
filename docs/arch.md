# cppws architecture

cppws will follow a distributed authority model, meaning that decisions are taken collectively by
nodes across the network through emergent behaviour, rather than by a central authority.

## Clarification on the term "cell"

In the context of this project and architecture at large, the term "_cell_" has been used as the architecture
itself has strong likeness to that of a biological organism (more specifically _animals_). A "_cell_" then,
is as in organisms, a functional unit of the larger system that contains genetic information and that is
capable of division.

- Genetic information in this context is the full server configuration, secret cryptographic keys, and both
innate and stored learned behaviours (I.e code and statistics).

- Functional unit refers to any type of module that contains both memory and routines (processes, threads,
fibers, etc.)

- Division in this context is process forking, or any other type of routine duplication.

- The _system_ or _organism_ can refer to the server cluster as a whole, or a single host in the server cluster.

## At initialization

What will happen at initialization of the system, is that an "egg cell" will be spawned by the system
administrator (cell being a single process/node in the network). This "egg cell" will provided the "_DNA_"
(server code) of the server, which will determine the behaviour of the system at large.

The _egg_ opens the server socket(s), and starts listening for requests. Unless a request is received by
the cell, it will remain dormant. Upon receiving a request however, the cell will begin taking actions.

The first action the _egg_ is likely to take upon receiving the first request, is to divide twice. The reason
for this is because processes in this "organic" environment are terminated by vote of majority.

Having just two processes would mean that there is no way of achieving a majority vote. Having just one cell
would also be a problem, as it means that there is no mode of recovery should it die (crash) or become
infected (compromised by threat actors).

The specialization would work as following for the initial cells:

- The mother cell (_egg_) becomes a neuron (passes executive decisions unto the network).
- First child becomes a "_blood cell_", meaning it passes messages to other cells and hosts (may also act as an "_antibody_").
- The second child becomes a "_drone_" or "_worker cell_". This means that it will be handling the actual request.

In addition to this, the mother cell will also spawn new stem cells in order to ensure that the stem cell class
does not die out. The exact quantity of stem cells that are spawned will be determined by the _DNA_.

## During runtime

Cells will divide and die according to some predefined ratio which is stored in the cell _DNA_ (configuration
and user code). Cells always start out as stem cells. Stem cells are cells that are yet to specify their
behaviour and remain idle inside a process pool until awoken by a "_hormone_" (signal).

Typically, neuron cells will be the ones that determine when a stem cell needs to transition into a specialized
role. Whenever a neuron (neurons actively monitor system resources) determines that the load is particularly high
it will release a signal that wakes up cells in the stem cell pool.

These cells then wake up and take on a role depending on the signaled needs of the system and rules that where
predefined by the _DNA_.

Cells may also choose to divide themselves. Cells that divide in this manner can only divide into cells of their
own respective class of cells (except stem cells). This means that any one cell only needs to respond to signals
that apply to their specialization.

## Cell type specification

As mentioned, cells have different assigned classes. These classes determine the final behaviour of the cell, and
what stimuli the cell will respond to. This section goes through in more detail what each class of cells do.

### Stem cells

Stem cells are idle processes/resources that wait for a signal to partake in some task. Cells only remain stem cells
until they awoken for the first time. Upon being signaled, the stem cell will assume a role (in accordance with it's
assigned _DNA_) and has after that point fundamentally changed.

There will typically be a pool of stem cells that is automatically replenishes itself to some equilibrium. These are
awoken whenever a _neuron_, or some other cell sends a hormone (signal/message) that indicates that resources are lacking.

The stem cell will then analyze the message. Depending on the strength of any one hormone (each type of cell class has
a "growth" hormone), the stem cell will be biased to turn into the cell class indicated by the hormone. To ensure that
all stem cells (and all cells in general) don't respond the same way to a hormone, the strength of the hormone is
attenuated for every cell it passes.

In general, a hormone is only passed through direct family hierarchy: mother to daughter or daughter to mother. For every
jump the hormone makes, it is attenuated by some factor (_DNA_ determined).

This way, "newer" stem cells receive a weaker signal, making it so that the growth does not reach unsustainable levels.

### Immune cells

Immune cells come in different variants, and are, like organic immune cells, responsible for detecting and eliminating
threats. The most basic form of immune cell is the "_B cell_". This type of cell acts as the security filter
layer of the server. It will accept incoming requests and determine whether or not they are safe to pass on to the rest
of the network.

Should a B cell find an incoming request to be illegitimate, it will attach a new signal to the external request that lets
the rest of the network know about the malicious request.

What follows next is the _T immune cell_. What sets T cells apart from all other cells is that it, unlike any other cell type,
will process requests that have been marked as malicious. When a request is marked as malicious by a B cell, it is passed on
(similar to a hormone) until it reaches a T cell. The T cell reads in the request, and adds it to an accumulated statistic about
requests that have been marked as malicious.

Upon having received and processed a marked request, the T cell will send out a new signal containing the updated security
statistics. This then determines the behaviour of all other cells in the network (requests from that IP range may be blocked
in the future, authorities may be contacted, etc.).

### Drones/worker cells

Drones (or worker cells) process requests that have been approved by immune cells. These are basically the parts of the cluster
that perform the actual logic of the server.

When a drone has processed a request, their response gets released to the network and is eventually caught by messenger (or _blood_)
cells. These then pass the data back unto the output socket of the server.

### Blood cells

_Blood cells_ (or "messenger cells") are cells specialized in message passing. These are used to bridge the gap between network components
and is the cell type responsible for passing messages to/from the socket of the server. When using a multi-machine cluster, blood cells are
also responsible for upholding communication between the different server hosts.

### Reproductive cells

To integrate or expand the size of the server cluster, you would just as in organic systems, "reproduce". This is done using a method that
can be likened to how animals would reproduce at a cellular level:

1. The new server host is implanted with an "_egg_" cell (this is to ensure the server only grows out to receptive hosts). The "egg" is set
to listen on some predefined port (see next step).
2. The source server sends over a packet of information to the new server through the open port (can be likened to a "spore" or "sperm" cell).
3. The egg on the new host receives the packet and is now initialized with the DNA of the server at large.
4. Once a "growth" hormone reaches the new host, the egg will divide as described earlier, and new cells start to grow.
5. Communication can now be upheld through regular "blood cells".

## Identifying foreign bodies

To identify foreign entities making their way into the server cluster, we can once again utilize the organic model. Given that all _cells_ in
the cluster all share _DNA_ (common server code and configuration), we can encode some key into the _DNA_ of the cells. Any foreign entity in
the cluster that is not able to provide this _key_ when prompted can be assumed to be an intruder.

This means that only _cells_ (processes and behaviours) that can prove their origin as one of the same organism will be allowed to work in the
server environment. If a foreign entity is discovered, it will be marked for termination and then later on neutralized by immune cells.

## Hormones

Hormones in the context of this server architecture are data packets that influence the behaviour of cells. Hormones typically do not immediately
determine cell behaviour. Cells will rather average over all received hormones, and will make their decisions based on how far weighted each type
of hormone is.

Hormones may be implemented using any data transfer mechanism, however, the more common and simple types will be implemented using POSIX _signals_.

The most basic types of _hormones_ are so called "growth hormones". These are signals that influence how likely cells are to replicate, or die.
If a large quantity of "growth hormones" are received, the receiving cell is likely to be duplicated. For every hormone there will typically also
be a type of "anti-hormone" the presence of which regulate the influence of the regular hormone.

### Hormone transfer

Hormones can be released by any type of cell, but may only be received by cells that have an immediate relation with the emitting cell. This will
typically mean that hormones are most often always sent mother-to-child or child-to-mother. Hormones are then distributed through the network by
passing on received messages to the next cells.

Each time a hormone is duplicated however, its effect strength will be decreased. This is to emulate dilution and to mitigate the power of any one
cell in the cluster. Because signals have reduced strength when passed over a large number of nodes, it would take the collective decision of many
cells to change the behaviour of the whole network.

