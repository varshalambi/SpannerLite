#pragma once
#include "../__dep__.h"
#include "../command.h"
#include "deptran/procedure.h"

namespace janus {


class TxData;
class TpcPrepareCommand : public Marshallable {
 public:
  TpcPrepareCommand() : Marshallable(MarshallDeputy::CMD_TPC_PREPARE) {
  }
  txnid_t tx_id_ = 0;
  int32_t ret_ = -1;
  shared_ptr<Marshallable> cmd_{nullptr};

  Marshal& ToMarshal(Marshal&) const override;
  Marshal& FromMarshal(Marshal&) override;
};

class TpcCommitCommand : public Marshallable {
 public:
  TpcCommitCommand() : Marshallable(MarshallDeputy::CMD_TPC_COMMIT) {
  }
  txnid_t tx_id_ = 0;
  int ret_ = -1;
  shared_ptr<Marshallable> cmd_{nullptr};
  virtual Marshal& ToMarshal(Marshal&) const override;
  virtual Marshal& FromMarshal(Marshal&) override;
};

class TpcEmptyCommand : public Marshallable {
 private:
  shared_ptr<BoxEvent<bool>> event{Reactor::CreateSpEvent<BoxEvent<bool>>()};

 public:
  TpcEmptyCommand() : Marshallable(MarshallDeputy::CMD_TPC_EMPTY) {}
  Marshal& ToMarshal(Marshal&) const override;
  Marshal& FromMarshal(Marshal&) override;
  void Wait() { event->Wait(); };
  void Done() { event->Set(1); };
};

class TpcNoopCommand : public Marshallable {
  public:
  TpcNoopCommand() : Marshallable(MarshallDeputy::CMD_NOOP) {}

  Marshal& ToMarshal(Marshal&) const override;
  Marshal& FromMarshal(Marshal&) override;
};

class TpcBatchCommand : public Marshallable {
  uint32_t size_ = 0;
public:
  TpcBatchCommand() : Marshallable(MarshallDeputy::CMD_TPC_BATCH) {}
  vector<shared_ptr<TpcCommitCommand> > cmds_;

  void AddCmd(shared_ptr<TpcCommitCommand> cmd);
  void AddCmds(vector<shared_ptr<TpcCommitCommand> >& cmds);
  void ClearCmd();
  inline size_t Size() const { return cmds_.size(); }
  
  Marshal& ToMarshal(Marshal&) const override;
  Marshal& FromMarshal(Marshal&) override;
};

} // namespace janus
