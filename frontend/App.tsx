
import React, { useState, useEffect, useRef } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { ViewType, SystemLog } from './types';
import { CustomerModule } from './components/CustomerModule';
import { MenuModule } from './components/MenuModule';
import { OrderModule } from './components/OrderModule';
import { KitchenModule } from './components/KitchenModule';
import { BillingModule } from './components/BillingModule';
import { HealthModule } from './components/HealthModule';
import { backendService } from './services/backendService';

const App: React.FC = () => {
  const [activeView, setActiveView] = useState<ViewType>('DASHBOARD');
  const [showConsole, setShowConsole] = useState(false);
  const [logs, setLogs] = useState<SystemLog[]>([]);
  const [time, setTime] = useState(new Date().toLocaleTimeString());
  const logEndRef = useRef<HTMLDivElement>(null);

  // Dashboard Stats
  const [stats, setStats] = useState({ orders: 0, customers: 0, revenue: 0 });

  useEffect(() => {
    const timer = setInterval(() => setTime(new Date().toLocaleTimeString()), 1000);
    const unsubscribe = backendService.subscribeToLogs((log) => {
      setLogs(prev => [...prev.slice(-49), log]);
    });
    
    const fetchStats = async () => {
      const [o, c, b] = await Promise.all([
        backendService.getActiveOrders(),
        backendService.getAllCustomers(),
        backendService.getAllBills()
      ]);
      setStats({
        orders: o.data?.length || 0,
        customers: c.data?.length || 0,
        revenue: b.data?.reduce((acc, curr) => curr.status === 'PAID' ? acc + curr.total : acc, 0) || 0
      });
    };
    fetchStats();

    return () => {
      clearInterval(timer);
      unsubscribe();
    };
  }, [activeView]);

  useEffect(() => {
    if (logEndRef.current) logEndRef.current.scrollIntoView({ behavior: 'smooth' });
  }, [logs]);

  const renderView = () => {
    switch(activeView) {
      case 'CUSTOMERS': return <CustomerModule />;
      case 'MENU': return <MenuModule />;
      case 'ORDERS': return <OrderModule />;
      case 'KITCHEN': return <KitchenModule />;
      case 'BILLING': return <BillingModule />;
      case 'HEALTH': return <HealthModule />;
      case 'DASHBOARD':
      default: return (
        <motion.div initial={{ opacity: 0, y: 20 }} animate={{ opacity: 1, y: 0 }} className="space-y-12">
          {/* SaaS Hero Header */}
          <div className="relative overflow-hidden glass-dark p-12 rounded-[3.5rem] border border-white/10 shadow-2xl">
            <div className="absolute top-0 right-0 w-96 h-96 bg-indigo-500/10 blur-[120px] rounded-full -mr-20 -mt-20"></div>
            <div className="relative z-10 flex flex-col lg:flex-row gap-8 items-center justify-between">
              <div className="space-y-4">
                <div className="flex items-center gap-3">
                  <span className="px-3 py-1 bg-indigo-500/20 text-indigo-400 text-[10px] font-black rounded-full tracking-widest uppercase border border-indigo-500/30">v2.9.4 Stable</span>
                  <span className="w-2 h-2 bg-emerald-500 rounded-full animate-pulse"></span>
                </div>
                <h3 className="text-6xl font-black text-white tracking-tighter italic uppercase leading-tight">
                  System <span className="text-transparent bg-clip-text bg-gradient-to-r from-indigo-400 to-cyan-400">Overview</span>
                </h3>
                <p className="text-slate-400 max-w-lg font-medium">Real-time telemetry and operation control for the high-performance C++ backend engine.</p>
              </div>
              <div className="flex items-center gap-12">
                <Telemetry label="Live_Orders" value={stats.orders.toString()} />
                <Telemetry label="Revenue_USD" value={`$${stats.revenue.toFixed(2)}`} />
                <div className="h-16 w-[1px] bg-white/10 hidden lg:block"></div>
                <div className="text-right">
                  <p className="text-[10px] text-slate-500 font-black uppercase tracking-widest mb-1">Station_Time</p>
                  <p className="text-4xl font-black text-white mono">{time}</p>
                </div>
              </div>
            </div>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
            <DashboardCard title="POS Terminal" desc="Seat guests & take orders" icon="🛒" onClick={() => setActiveView('ORDERS')} color="indigo" />
            <DashboardCard title="Fire Deck" desc="Kitchen production fire-board" icon="🔥" onClick={() => setActiveView('KITCHEN')} color="amber" />
            <DashboardCard title="Menu Engine" desc="Manage inventory & catalog" icon="📂" onClick={() => setActiveView('MENU')} color="cyan" />
            <DashboardCard title="Guest Book" desc="Customer CRM and directory" icon="👥" onClick={() => setActiveView('CUSTOMERS')} color="emerald" />
            <DashboardCard title="Settlements" desc="Bill generation & processing" icon="💰" onClick={() => setActiveView('BILLING')} color="rose" />
            <DashboardCard title="System Health" desc="C++ core diagnostics & I/O" icon="🩺" onClick={() => setActiveView('HEALTH')} color="slate" />
          </div>
        </motion.div>
      );
    }
  };

  return (
    <div className="h-screen w-screen flex flex-col md:flex-row overflow-hidden bg-[#020617] text-slate-200">
      <nav className="w-full md:w-28 bg-[#050811] border-r border-white/5 flex flex-col items-center py-10 z-[60]">
        <div className="mb-14 cursor-pointer" onClick={() => setActiveView('DASHBOARD')}>
          <div className="w-16 h-16 bg-gradient-to-br from-indigo-600 to-indigo-800 rounded-[1.5rem] flex items-center justify-center font-black text-3xl shadow-2xl shadow-indigo-600/30 active:scale-95 transition-transform">R</div>
        </div>
        <div className="flex-1 space-y-8 flex flex-col items-center">
          <NavBtn active={activeView === 'DASHBOARD'} onClick={() => setActiveView('DASHBOARD')} icon="🏠" label="Overview" />
          <NavBtn active={activeView === 'ORDERS'} onClick={() => setActiveView('ORDERS')} icon="🛒" label="POS" />
          <NavBtn active={activeView === 'KITCHEN'} onClick={() => setActiveView('KITCHEN')} icon="🔥" label="Kitchen" />
          <NavBtn active={activeView === 'BILLING'} onClick={() => setActiveView('BILLING')} icon="💰" label="Billing" />
          <NavBtn active={activeView === 'HEALTH'} onClick={() => setActiveView('HEALTH')} icon="🩺" label="Health" />
        </div>
        <button 
          onClick={() => setShowConsole(!showConsole)} 
          className={`group mt-auto w-14 h-14 rounded-2xl border transition-all flex items-center justify-center relative ${showConsole ? 'bg-indigo-600 border-indigo-500 text-white' : 'border-white/10 text-slate-500 hover:text-white'}`}
        >
          <span className="mono font-bold">{">_"}</span>
          <div className="absolute left-full ml-4 px-4 py-2 bg-slate-900 border border-white/10 rounded-xl opacity-0 invisible group-hover:opacity-100 group-hover:visible transition-all duration-300 whitespace-nowrap z-[100] shadow-2xl">
            <p className="text-[10px] font-black uppercase tracking-[0.2em] text-white">Terminal</p>
          </div>
        </button>
      </nav>

      <main className="flex-1 flex flex-col relative overflow-hidden">
        <header className="h-28 px-14 flex items-center justify-between border-b border-white/5 bg-[#020617]/50 backdrop-blur-3xl z-50">
          <div className="flex items-center gap-8">
            <h2 className="text-2xl font-black text-white uppercase tracking-[0.2em] italic leading-none">{activeView}</h2>
            <div className="px-5 py-2 bg-indigo-500/10 border border-indigo-500/20 rounded-full text-[10px] font-black text-indigo-400 tracking-widest uppercase">Kernel_Connected</div>
          </div>
          <div className="flex items-center gap-8">
            <div className="flex flex-col items-end">
              <span className="text-[9px] font-black text-slate-500 uppercase tracking-widest">I/O_Load</span>
              <div className="w-24 h-1.5 bg-white/5 rounded-full mt-1 overflow-hidden">
                <motion.div initial={{ width: "0%" }} animate={{ width: "45%" }} className="h-full bg-indigo-500"></motion.div>
              </div>
            </div>
            <div className="flex items-center gap-4">
              <div className="w-2.5 h-2.5 bg-emerald-500 rounded-full animate-pulse shadow-[0_0_15px_#10b981]"></div>
              <p className="text-[11px] font-black text-slate-400 uppercase tracking-widest">Node_01: OK</p>
            </div>
          </div>
        </header>

        <div className="flex-1 overflow-y-auto p-14 scrollbar-hide bg-[radial-gradient(circle_at_top_right,rgba(99,102,241,0.06),transparent_50%)]">
          <AnimatePresence mode="wait">
            <motion.div key={activeView} initial={{ opacity: 0, x: 20 }} animate={{ opacity: 1, x: 0 }} exit={{ opacity: 0, x: -20 }} transition={{ duration: 0.4 }} className="max-w-7xl mx-auto pb-32">
              {renderView()}
            </motion.div>
          </AnimatePresence>
        </div>

        <AnimatePresence>
          {showConsole && (
            <motion.div initial={{ y: "100%" }} animate={{ y: 0 }} exit={{ y: "100%" }} className="absolute bottom-0 left-0 right-0 h-96 glass-dark border-t border-white/10 z-[100] flex flex-col">
              <div className="h-12 px-8 flex items-center justify-between bg-white/5 border-b border-white/5">
                <span className="text-[11px] font-black text-indigo-400 tracking-widest uppercase italic">Backend System Terminal</span>
                <button onClick={() => setShowConsole(false)} className="text-slate-500 hover:text-white transition-colors">
                  <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path strokeLinecap="round" strokeLinejoin="round" strokeWidth="3" d="M19 9l-7 7-7-7" /></svg>
                </button>
              </div>
              <div className="flex-1 p-8 overflow-y-auto mono text-[12px] space-y-2 scrollbar-hide bg-black/60">
                {logs.map((log, i) => (
                  <div key={i} className="flex gap-6 group opacity-80 hover:opacity-100 transition-opacity">
                    <span className="text-slate-600 shrink-0">[{log.timestamp}]</span>
                    <span className={`font-bold shrink-0 w-16 ${log.level === 'ERROR' ? 'text-rose-500' : log.level === 'WARN' ? 'text-amber-500' : log.level === 'IO' ? 'text-indigo-400' : 'text-emerald-500'}`}>[{log.level}]</span>
                    <span className="text-slate-300">{log.message}</span>
                  </div>
                ))}
                <div ref={logEndRef} />
              </div>
            </motion.div>
          )}
        </AnimatePresence>
      </main>
    </div>
  );
};

const Telemetry = ({ label, value }: { label: string, value: string }) => (
  <div className="text-right">
    <p className="text-[10px] text-slate-500 font-black uppercase tracking-widest mb-1">{label}</p>
    <p className="text-3xl font-black text-white mono leading-none tracking-tighter">{value}</p>
  </div>
);

const NavBtn = ({ active, onClick, icon, label }: { active: boolean, onClick: () => void, icon: string, label: string }) => (
  <button 
    onClick={onClick} 
    className={`w-16 h-16 rounded-[1.5rem] flex items-center justify-center text-2xl transition-all duration-500 relative group ${active ? 'bg-white/5 text-white shadow-inner' : 'text-slate-600 hover:text-slate-300 hover:bg-white/5'}`}
  >
    {icon}
    {active && <motion.div layoutId="nav-glow" className="absolute left-0 w-1.5 h-8 bg-indigo-500 rounded-full shadow-[0_0_15px_#6366f1]" />}
    <div className="absolute left-full ml-4 px-4 py-2 bg-slate-900 border border-white/10 rounded-xl opacity-0 invisible group-hover:opacity-100 group-hover:visible translate-x-[-10px] group-hover:translate-x-0 transition-all duration-300 whitespace-nowrap z-[100] shadow-2xl">
      <p className="text-[10px] font-black uppercase tracking-[0.2em] text-white leading-none">{label}</p>
      <div className="absolute top-1/2 -left-1 -translate-y-1/2 w-2 h-2 bg-slate-900 border-l border-b border-white/10 rotate-45"></div>
    </div>
  </button>
);

const DashboardCard = ({ title, desc, icon, onClick, color }: any) => {
  const themeClasses: Record<string, string> = {
    indigo: 'hover:border-indigo-500/50 hover:shadow-[0_20px_80px_rgba(99,102,241,0.15)] group-hover:bg-indigo-500/5',
    amber: 'hover:border-amber-500/50 hover:shadow-[0_20px_80px_rgba(245,158,11,0.15)] group-hover:bg-amber-500/5',
    cyan: 'hover:border-cyan-500/50 hover:shadow-[0_20px_80px_rgba(6,182,212,0.15)] group-hover:bg-cyan-500/5',
    emerald: 'hover:border-emerald-500/50 hover:shadow-[0_20px_80px_rgba(16,185,129,0.15)] group-hover:bg-emerald-500/5',
    rose: 'hover:border-rose-500/50 hover:shadow-[0_20px_80px_rgba(244,63,94,0.15)] group-hover:bg-rose-500/5',
    slate: 'hover:border-slate-500/50 hover:shadow-[0_20px_80px_rgba(100,116,139,0.15)] group-hover:bg-slate-500/5',
  };

  const iconColors: Record<string, string> = {
    indigo: 'group-hover:text-indigo-400 group-hover:bg-indigo-500/20',
    amber: 'group-hover:text-amber-400 group-hover:bg-amber-500/20',
    cyan: 'group-hover:text-cyan-400 group-hover:bg-cyan-500/20',
    emerald: 'group-hover:text-emerald-400 group-hover:bg-emerald-500/20',
    rose: 'group-hover:text-rose-400 group-hover:bg-rose-500/20',
    slate: 'group-hover:text-slate-400 group-hover:bg-slate-500/20',
  };

  return (
    <button 
      onClick={onClick} 
      className={`group relative p-12 rounded-[3.5rem] border border-white/5 bg-white/[0.02] transition-all duration-500 text-left h-80 flex flex-col justify-end overflow-hidden active:scale-95 hover:-translate-y-2 ${themeClasses[color]}`}
    >
      <div className={`absolute top-12 left-12 w-20 h-20 bg-white/5 rounded-[2rem] flex items-center justify-center text-4xl transition-all duration-700 group-hover:scale-110 group-hover:-rotate-6 ${iconColors[color]}`}>
        {icon}
      </div>
      <div className="relative z-10 transition-transform duration-500 group-hover:translate-x-2">
        <h3 className="text-4xl font-black text-white tracking-tighter italic leading-none transition-colors group-hover:text-white">{title}</h3>
        <p className="text-slate-400 text-sm mt-4 font-medium opacity-60 group-hover:opacity-100 transition-all duration-500">{desc}</p>
        <div className="mt-8 flex items-center gap-3 text-[10px] font-black text-slate-500 uppercase tracking-[0.3em] transition-all duration-500 group-hover:text-white">
          Enter Interface
          <svg className="w-5 h-5 transition-transform duration-500 group-hover:translate-x-3" fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="3" d="M17 8l4 4m0 0l-4 4m4-4H3" />
          </svg>
        </div>
      </div>
      <div className="absolute -bottom-20 -right-20 w-64 h-64 bg-white/5 rounded-full blur-[100px] opacity-0 group-hover:opacity-20 transition-opacity duration-700"></div>
    </button>
  );
};

export default App;
