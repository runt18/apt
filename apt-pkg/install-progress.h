#ifndef PKGLIB_IPROGRESS_H
#define PKGLIB_IPROGRESS_H

#include <apt-pkg/macros.h>

#include <string>
#include <unistd.h>
#include <signal.h>
#include <vector>

namespace APT {
namespace Progress {

 class PackageManager;
 PackageManager* PackageManagerProgressFactory();

 class PackageManager
 {
 private:
    /** \brief dpointer placeholder */
    void *d;

 protected:
    std::string progress_str;
    float percentage;
    int last_reported_progress;

 public:
    PackageManager();
    virtual ~PackageManager();

    /* Global Start/Stop */
    virtual void Start(int /*child_pty*/=-1) {};
    virtual void Stop() {};

    /* When dpkg is invoked (may happen multiple times for each
     * install/remove block
    */
    virtual void StartDpkg() {};

    virtual pid_t fork() {return fork(); };

    virtual void Pulse() {};
    virtual long GetPulseInterval() {
         return 500000;
    };

    virtual bool StatusChanged(std::string PackageName,
                               unsigned int StepsDone,
                               unsigned int TotalSteps,
                               std::string HumanReadableAction);
    virtual void Error(std::string /*PackageName*/,
                       unsigned int /*StepsDone*/,
                       unsigned int /*TotalSteps*/,
                       std::string /*ErrorMessage*/) {}
    virtual void ConffilePrompt(std::string /*PackageName*/,
                                unsigned int /*StepsDone*/,
                                unsigned int /*TotalSteps*/,
                                std::string /*ConfMessage*/) {}
 };

 class PackageManagerProgressFd : public PackageManager
 {
    void *d;
 protected:
    int OutStatusFd;
    int StepsDone;
    int StepsTotal;
    void WriteToStatusFd(std::string msg);

 public:
    explicit PackageManagerProgressFd(int progress_fd);
    virtual ~PackageManagerProgressFd();

    virtual void StartDpkg();
    virtual void Stop();

    virtual bool StatusChanged(std::string PackageName,
                               unsigned int StepsDone,
                               unsigned int TotalSteps,
                               std::string HumanReadableAction);
    virtual void Error(std::string PackageName,
                       unsigned int StepsDone,
                       unsigned int TotalSteps,
                          std::string ErrorMessage);
    virtual void ConffilePrompt(std::string PackageName,
                                unsigned int StepsDone,
                                unsigned int TotalSteps,
                                   std::string ConfMessage);

 };

 class PackageManagerProgressDeb822Fd : public PackageManager
 {
    void *d;
 protected:
    int OutStatusFd;
    int StepsDone;
    int StepsTotal;
    void WriteToStatusFd(std::string msg);

 public:
    explicit PackageManagerProgressDeb822Fd(int progress_fd);
    virtual ~PackageManagerProgressDeb822Fd();

    virtual void StartDpkg();
    virtual void Stop();

    virtual bool StatusChanged(std::string PackageName,
                               unsigned int StepsDone,
                               unsigned int TotalSteps,
                               std::string HumanReadableAction);
    virtual void Error(std::string PackageName,
                       unsigned int StepsDone,
                       unsigned int TotalSteps,
                          std::string ErrorMessage);
    virtual void ConffilePrompt(std::string PackageName,
                                unsigned int StepsDone,
                                unsigned int TotalSteps,
                                   std::string ConfMessage);
 };

 class PackageManagerFancy : public PackageManager
 {
    void *d;
 private:
    APT_HIDDEN static void staticSIGWINCH(int);
    static std::vector<PackageManagerFancy*> instances;
    APT_HIDDEN bool DrawStatusLine();

 protected:
    void SetupTerminalScrollArea(int nr_rows);
    void HandleSIGWINCH(int);

    typedef struct {
       int rows;
       int columns;
    } TermSize;
    TermSize GetTerminalSize();

    sighandler_t old_SIGWINCH;
    int child_pty;

 public:
    PackageManagerFancy();
    virtual ~PackageManagerFancy();
    virtual void Start(int child_pty=-1);
    virtual void Stop();
    virtual bool StatusChanged(std::string PackageName,
                               unsigned int StepsDone,
                               unsigned int TotalSteps,
                               std::string HumanReadableAction);

    // return a progress bar of the given size for the given progress 
    // percent between 0.0 and 1.0 in the form "[####...]"
    static std::string GetTextProgressStr(float percent, int OutputSize);
 };

 class PackageManagerText : public PackageManager
 {
    void *d;
 public:
    virtual bool StatusChanged(std::string PackageName,
                               unsigned int StepsDone,
                               unsigned int TotalSteps,
                               std::string HumanReadableAction);

    PackageManagerText();
    virtual ~PackageManagerText();
 };


} // namespace Progress
} // namespace APT

#endif
