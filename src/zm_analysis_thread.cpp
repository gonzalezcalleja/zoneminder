#include "zm_analysis_thread.h"

#include "zm_monitor.h"
#include "zm_signal.h"
#include "zm_time.h"

AnalysisThread::AnalysisThread(Monitor *monitor) :
    monitor_(monitor), terminate_(false) {
  thread_ = std::thread(&AnalysisThread::Run, this);
}

AnalysisThread::~AnalysisThread() {
  Stop();
  if (thread_.joinable()) thread_.join();
}

void AnalysisThread::Start() {
  if (thread_.joinable()) thread_.join();
  terminate_ = false;
  Debug(3, "Starting analysis thread");
  thread_ = std::thread(&AnalysisThread::Run, this);
}

void AnalysisThread::Run() {
  while (!(terminate_ or zm_terminate)) {
    // Some periodic updates are required for variable capturing framerate
    Debug(2, "Analyzing");
    if (!monitor_->Analyse()) {
      if (!(terminate_ or zm_terminate)) {
        Microseconds sleep_for = monitor_->Active() ? Microseconds(ZM_SAMPLE_RATE) : Microseconds(ZM_SUSPENDED_RATE);
        Debug(2, "Sleeping for %" PRId64 "us", int64(sleep_for.count()));
        std::this_thread::sleep_for(sleep_for);
      }
    }
  }
}
