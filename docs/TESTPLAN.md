# TestPlan and Milestones
This incremental test plan ensures the simtemp Linux kernel module is validated step-by-step for stability, correctness, and safe integration into production systems.
## T0 - Baseline Module
Goal: Verify the module can be built, loaded, and unloaded cleanly.
- [x] Build Module
- [x] Load (insmod) → Inspect dmesg for init message
- [x] Unload (rmmod) → Inspect dmesg for exit message
- [x] Confirm no kernel warnings (dmesg -T | grep -i warn)
- [x] Add Lint to ensure good code quality.
## T1 - Test Module
Goal: Ensure device nodes and sysfs entries are correctly registered
- [x] Build without warnings
- [x] Load the module
- [x] Confirm /dev/simtemp
- [x] cat /dev/simtemp
- [x] Confirm sysfs attributes (/sys/class/...) exist
- [x] Unload and confirm cleanup (no lingering device nodes or sysfs entries)
## T2 Data Generation
Goal: Verify data generation and sampling behavior. 
- [x] Confirm default periodic data updates (baseline period)
- [x] Change sampling time to 100 ms
- [x] Verify periodicity with strace or poll() timing
- [x] Ensure module handles timing drift under long runs (e.g., 5–10s test)
## T3 Threshold Event
Goal: Verify blocking reads and threshold triggers.
- [x] Lower threshold below mean value
- [x] Confirm poll()/blocking read unblocks within 2–3 periods
- [x] Verify event flag/notification is set and cleared correctly.
- [x] Test boundary conditions (threshold exactly equal to mean)
## T4 Error Paths:
Goal: Ensure invalid operations are safely rejected.
- [x] Invalid writes return -EINVAL
- [x] Invalid ioctl returns -ENOTTY (if applicable)
- [ ] Stress test under fast sampling (1 ms) → confirm stability, no kernel warnings
- [x] Verify correct cleanup on unexpected failures (rmmod during active read)
## T5 Concurrency:
Goal: Test thread safety and safe teardown.
- [x] Run reader + config writer concurrently
- [x] Confirm no race conditions (e.g., inconsistent reads)
- [x] Confirm no deadlocks (system remains responsive)
- [x] Verify safe unload while multiple readers/writers are active (graceful fail/close)

## T6 API Contract:
Goal: Ensure stable user–kernel interface.
- [x] Verify struct layout (alignment, padding, endianness)
- [x] Document expected read/write formats
- [ ] User-space app handles partial reads/writes robustly
- [ ] Confirm API stability across rebuilds (struct size must not change silently)