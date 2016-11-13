#include "remote.h"
#include <pthread.h>
#include <gflags/gflags.h>

#include "proto/remote.pb.h"
#include "splits_screen.h"
#include "google/protobuf/text_format.h"
#include "util/file.h"
#include "util/logging.h"
#include "util/status.h"

using google::protobuf::TextFormat;

DEFINE_string(remote_fifo, "/tmp/z2splits",
              "Name of named pipe for remote commands");

Remote::Remote(SplitsScreen* splits) : splits_(splits) {}

void Remote::Start() {
    util::Status status;

    status = File::MkFifo(FLAGS_remote_fifo);
    if (!status.ok()) {
        LOG(ERROR, "Unable to create fifo: ", status.ToString());
    }

    pthread_t thr;
    pthread_create(&thr, nullptr, &Remote::ThreadRunner, (void*)this);
}

void Remote::Run() {
    for(;;) {
        std::unique_ptr<File> file = File::Open(FLAGS_remote_fifo, "r");
        std::string textpb;

        while(file->ReadLine(&textpb)) {
            z2splits::RemoteCommand cmd;
            if (textpb == "") {
                break;
            }
            if (!TextFormat::ParseFromString(textpb, &cmd)) {
                LOG(ERROR, "Could not parse remote command: ", textpb);
                textpb.clear();
                continue;
            }
            textpb.clear();
            LOG(INFO, "remote command: ", cmd.DebugString());

            if (cmd.command() == "reset") {
                splits_->reset();
            } else if (cmd.command() == "start") {
                splits_->go();
            } else if (cmd.command() == "split") {
                splits_->next(cmd.data(0));
            } else {
                LOG(ERROR, "Unknown remote command: ", cmd.DebugString());
            }
        }
    }
}

void* Remote::ThreadRunner(void *arg) {
    Remote *that = (Remote*)arg;
    that->Run();
    return nullptr;
}
