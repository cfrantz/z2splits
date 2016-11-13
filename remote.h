#ifndef Z2SPLITS_REMOTE_H
#define Z2SPLITS_REMOTE_H

class SplitsScreen;

class Remote {
  public:
    Remote(SplitsScreen* splits);
    void Start();
    void Run();
  private:
    SplitsScreen* splits_;
    static void *ThreadRunner(void *arg);
};

#endif // Z2SPLITS_REMOTE_H
