import m5
from m5.objects import *
from caches import *
from CustomSIMDFU import *

system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock='5GHz'
system.clk_domain.voltage_domain=VoltageDomain()

system.mem_mode = 'timing'               # Use timing accesses
system.mem_ranges = [AddrRange('1GB')] # Create an address range

num_cpu = 4
system.cpu = [X86MinorCPU() for _ in range(num_cpu)]

for i in range(num_cpu):
    system.cpu[i].executeFuncUnits =  CustomFUPool()

# Adding our caches to this system
for i in range(num_cpu):
    system.cpu[i].icache = L1ICache()
    system.cpu[i].dcache = L1DCache()
    system.cpu[i].icache.connectCPU(system.cpu[i])
    system.cpu[i].dcache.connectCPU(system.cpu[i])

# We can't directly connect the L1 caches to the L2 cache since the L2 cache only expects a single port to connect to it.
# Therefore, we need to create an L2 bus toconnect out L1 caches to the L2 cache. Then we can use our helper function to connect the L1 caches to the L2 bus.
system.l2bus = L2XBar()

# Hook the CPU ports up to the l2bus
for i in range(num_cpu):
    system.cpu[i].icache.connectBus(system.l2bus)
    system.cpu[i].dcache.connectBus(system.l2bus)

# create our two l2 cache and connect it to the L2 bus
system.l2cache1 = L2Cache()
system.l2cache1.connectCPUSideBus(system.l2bus)

system.membus= SystemXBar()
system.l2cache1.connectMemSideBus(system.membus)

#interrupt controller
for i in range(num_cpu):
    system.cpu[i].createInterruptController()
    system.cpu[i].interrupts[0].pio = system.membus.mem_side_ports
    system.cpu[i].interrupts[0].int_requestor = system.membus.cpu_side_ports
    system.cpu[i].interrupts[0].int_responder = system.membus.mem_side_ports

# Create a DDR3 memory controller and connect it to the membus
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

# Connect the system up to the membus
system.system_port = system.membus.cpu_side_ports

process = Process()
binary = "/Users/nischaljsh/gem5/configs/assignment6/multithreaded_daxpy_x86"

for i in range(num_cpu):
    # Add unique arguments for each CPU thread (if needed)
    process.cmd = [binary] + [str(i)]  # Passing the CPU index as an argument
    system.cpu[i].workload = process
    system.cpu[i].createThreads()

system.workload = SEWorkload.init_compatible(binary)

# set up the root SimObject and start the simulation
root = Root(full_system = False, system = system)
# instantiate all of the objects we've created above
m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause()))
