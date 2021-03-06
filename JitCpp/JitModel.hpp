#pragma once
#include <Control/DefaultEffectItem.hpp>
#include <Process/Process.hpp>
#include <Process/GenericProcessFactory.hpp>
#include <Process/Process.hpp>
#include <Process/ProcessMetadata.hpp>

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>

#include <QDialog>

#include <Effect/EffectFactory.hpp>
#include <Process/Execution/ProcessComponent.hpp>
namespace Jit
{
class JitEffectModel;
}

PROCESS_METADATA(
    , Jit::JitEffectModel
    , "0a3b49d6-4ce7-4668-aec3-9505b6ee1a60"
    , "Jit"
    , "C++ Jit process"
    , Process::ProcessCategory::Script
    , "Script"
    , "JIT compilation process"
    , "ossia score"
    , QStringList{}
    , {}
    , {}
    , Process::ProcessFlags::ExternalEffect)
namespace Jit
{
template<typename Fun_T>
struct Driver;

using NodeCompiler = Driver<ossia::graph_node*()>;
using NodeFactory = std::function<ossia::graph_node*()>;

class JitEffectModel : public Process::ProcessModel
{
  friend class JitUI;
  friend class JitUpdateUI;
  SCORE_SERIALIZE_FRIENDS
  PROCESS_METADATA_IMPL(JitEffectModel)

  W_OBJECT(JitEffectModel)
public:
  JitEffectModel(
      TimeVal t, const QString& jitProgram, const Id<Process::ProcessModel>&,
      QObject* parent);
  ~JitEffectModel() override;

  JitEffectModel(DataStream::Deserializer& vis, QObject* parent);
  JitEffectModel(JSONObject::Deserializer& vis, QObject* parent);
  JitEffectModel(DataStream::Deserializer&& vis, QObject* parent);
  JitEffectModel(JSONObject::Deserializer&& vis, QObject* parent);

  const QString& script() const
  {
    return m_text;
  }

  static constexpr bool hasExternalUI() noexcept
  {
    return true;
  }

  QString prettyName() const noexcept override;
  void setScript(const QString& txt);

  Process::Inlets& inlets()
  {
    return m_inlets;
  }
  Process::Outlets& outlets()
  {
    return m_outlets;
  }

  NodeFactory factory;

  void errorMessage(const QString& e) W_SIGNAL(errorMessage, e)
private:
  void init();
  void reload();
  QString m_text;
  std::unique_ptr<NodeCompiler> m_compiler;
};
}

namespace Process
{
template <>
QString
EffectProcessFactory_T<Jit::JitEffectModel>::customConstructionData() const;

template <>
Process::Descriptor
EffectProcessFactory_T<Jit::JitEffectModel>::descriptor(QString d) const;
}
class QPlainTextEdit;
namespace Jit
{
struct JitEditDialog : public QDialog
{
  const JitEffectModel& m_effect;

  QPlainTextEdit* m_textedit{};
  QPlainTextEdit* m_error{};

public:
  JitEditDialog(
      const JitEffectModel& e, const score::DocumentContext& ctx,
      QWidget* parent);

  QString text() const;
};

using JitEffectFactory = Process::EffectProcessFactory_T<JitEffectModel>;
using LayerFactory = Process::EffectLayerFactory_T<
    JitEffectModel, Media::Effect::DefaultEffectItem, JitEditDialog>;
}

namespace Execution
{
class JitEffectComponent final : public Execution::ProcessComponent_T<
                                     Jit::JitEffectModel, ossia::node_process>
{
  COMPONENT_METADATA("122ceaeb-cbcc-4808-91f2-1929e3ca8292")

public:
  static constexpr bool is_unique = true;

  JitEffectComponent(
      Jit::JitEffectModel& proc, const Execution::Context& ctx,
      const Id<score::Component>& id, QObject* parent);
  ~JitEffectComponent() override;
};
using JitEffectComponentFactory
    = Execution::ProcessComponentFactory_T<JitEffectComponent>;
}
