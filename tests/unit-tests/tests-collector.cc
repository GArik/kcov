#include "test.hh"
#include "mocks/mock-engine.hh"

#include <elf.hh>
#include <collector.hh>
#include <utils.hh>

#include <string>

using namespace kcov;

class MockCollectorListener : public ICollector::IListener
{
public:
	MOCK_METHOD1(onBreakpoint, void(unsigned long addr));
};

TEST(collector)
{
	MockEngine &engine = (MockEngine &)IEngine::getInstance();
	MockCollectorListener listener;
	IElf *elf;
	char filename[1024];
	bool res;

	sprintf(filename, "%s/test-binary", crpcut::get_start_dir());
	elf = IElf::open(filename);
	ASSERT_TRUE(elf);

	ICollector &collector = ICollector::create(elf);

	collector.registerListener(listener);

	EXPECT_CALL(engine, setBreakpoint(_))
		.Times(AtLeast(1))
		;

	res = elf->parse();
	ASSERT_TRUE(res);

	int v;

	EXPECT_CALL(engine, start(_))
		.Times(Exactly(1))
		.WillRepeatedly(Return(true))
		;

	IEngine::Event evExit, evOnce;

	evExit.addr = 0;
	evExit.type = ev_exit;
	evExit.data = 1;

	evOnce.addr = 1;
	evOnce.type = ev_breakpoint;
	evOnce.data = 1;

	EXPECT_CALL(listener, onBreakpoint(1))
		.Times(Exactly(1))
		;

	EXPECT_CALL(engine, continueExecution())
		.Times(Exactly(2))
		.WillOnce(Return(evOnce))
		.WillRepeatedly(Return(evExit))
		;
	EXPECT_CALL(engine, clearBreakpoint(evOnce.data))
		.Times(Exactly(1))
		;

	v = collector.run();

	ASSERT_TRUE(v == evExit.data);

	evOnce.type = ev_error;

	// Test error
	EXPECT_CALL(engine, start(_))
		.Times(Exactly(1))
		.WillRepeatedly(Return(0))
		;

	EXPECT_CALL(engine, continueExecution())
		.Times(Exactly(1))
		.WillRepeatedly(Return(evOnce))
		;
	v = collector.run();

	ASSERT_TRUE(v == -1);
}
