#include "behaviortree_cpp_v3/bt_factory.h"

#include "dummy_nodes.h"
#include "movebase_node.h"

using namespace BT;

// clang-format off
static const char* xml_text = R"(
 <root main_tree_to_execute = "MainTree" >
     <BehaviorTree ID="MainTree">
        <Sequence name="root">
            <ThinkRuntimePort   text="{the_answer}"/>
            <SayRuntimePort     message="{the_answer}" />
        </Sequence>
     </BehaviorTree>
 </root>
 )";
// clang-format on

class ThinkRuntimePort: public BT::SyncActionNode
{
  public:
  ThinkRuntimePort(const std::string& name,
               const BT::NodeConfiguration& config)
    : BT::SyncActionNode(name, config)
  {
  }

  BT::NodeStatus tick() override {
    setOutput("text", "The answer is 42" );
    return NodeStatus::SUCCESS;
  }
};

class SayRuntimePort : public BT::SyncActionNode
{
  public:
  SayRuntimePort(const std::string& name, const BT::NodeConfiguration& config)
    : BT::SyncActionNode(name, config)
  {
  }

  // You must override the virtual function tick()
  BT::NodeStatus tick() override
  {
    auto msg = getInput<std::string>("message");
    if (!msg){
      throw BT::RuntimeError( "missing required input [message]: ", msg.error() );
    }
    std::cout << "Robot says: " << msg.value() << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};


int main()
{
    using namespace DummyNodes;

    BehaviorTreeFactory factory;

    //-------- register ports that might defined at runtime --------
    {
      // more verbose way
      PortsList ports = {BT::OutputPort<std::string>("text")};
      factory.registerBuilder(CreateManifest<ThinkRuntimePort>("ThinkRuntimePort", ports),
                              CreateBuilder<ThinkRuntimePort>());
    }

    {
      // less verbose way
      PortsList ports = {BT::InputPort<std::string>("message")};
      factory.registerNodeType<SayRuntimePort>("SayRuntimePort", ports);
    }

    auto tree = factory.createTreeFromText(xml_text);

    tree.root_node->executeTick();

    return 0;
}


