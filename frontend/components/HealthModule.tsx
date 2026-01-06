
import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';

export const HealthModule: React.FC = () => {
  const [load, setLoad] = useState(42);
  
  useEffect(() => {
    const interval = setInterval(() => {
      setLoad(prev => Math.max(10, Math.min(95, prev + (Math.random() * 10 - 5))));
    }, 2000);
    return () => clearInterval(interval);
  }, []);

  return (
    <div className="space-y-12">
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
        <HealthCard title="Kernel Status" status="Healthy" value="99.9% Uptime" icon="🧠" color="emerald" />
        <HealthCard title="I/O Throughput" status="Optimal" value="1.2 GB/s" icon="⚡" color="indigo" />
        <HealthCard title="Disk Storage" status="Syncing" value="482.1 MB" icon="💾" color="cyan" />
      </div>

      <div className="glass-dark p-12 rounded-[3.5rem] border border-white/5">
        <h3 className="text-2xl font-black text-white italic uppercase tracking-tighter mb-10">Real-Time Load Diagnostics</h3>
        <div className="space-y-12">
          <div className="space-y-4">
            <div className="flex justify-between items-end">
              <span className="text-sm font-black text-slate-500 uppercase tracking-widest">CPU Allocation (Backend Core)</span>
              <span className="mono text-2xl font-black text-white">{load.toFixed(1)}%</span>
            </div>
            <div className="w-full h-4 bg-white/5 rounded-full overflow-hidden p-1 border border-white/5">
              <motion.div 
                initial={{ width: "0%" }}
                animate={{ width: `${load}%` }}
                className={`h-full rounded-full transition-colors duration-1000 ${load > 80 ? 'bg-rose-500 shadow-[0_0_20px_rgba(244,63,94,0.5)]' : 'bg-indigo-500 shadow-[0_0_20px_rgba(99,102,241,0.5)]'}`}
              ></motion.div>
            </div>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-2 gap-12">
            <div className="space-y-6">
              <h4 className="text-[10px] font-black text-slate-500 uppercase tracking-[0.4em]">Subsystem Checks</h4>
              <Subsystem label="FSM Engine" status="Online" />
              <Subsystem label="IO Synchronization" status="Syncing" />
              <Subsystem label="Memory Buffer" status="Stable" />
              <Subsystem label="Persistence Layer" status="Ready" />
            </div>
            <div className="flex flex-col items-center justify-center p-8 bg-white/[0.02] border border-dashed border-white/10 rounded-[2rem]">
               <div className="w-24 h-24 border-8 border-indigo-500/20 border-t-indigo-500 rounded-full animate-spin"></div>
               <p className="mt-6 text-[11px] font-black text-slate-400 uppercase tracking-widest animate-pulse text-center">Engine actively monitoring filesystem stream...</p>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

const HealthCard = ({ title, status, value, icon, color }: any) => (
  <div className="glass-dark p-10 rounded-[3rem] border border-white/5 flex flex-col justify-between h-64">
    <div className="flex justify-between items-start">
      <div className="w-16 h-16 bg-white/5 rounded-2xl flex items-center justify-center text-3xl">{icon}</div>
      <span className={`px-4 py-1.5 rounded-full text-[9px] font-black uppercase tracking-widest ${color === 'emerald' ? 'bg-emerald-500/10 text-emerald-400' : color === 'indigo' ? 'bg-indigo-500/10 text-indigo-400' : 'bg-cyan-500/10 text-cyan-400'}`}>
        {status}
      </span>
    </div>
    <div>
      <h4 className="text-slate-500 text-[10px] font-black uppercase tracking-widest mb-1">{title}</h4>
      <p className="text-3xl font-black text-white tracking-tighter italic">{value}</p>
    </div>
  </div>
);

const Subsystem = ({ label, status }: { label: string, status: string }) => (
  <div className="flex justify-between items-center group">
    <span className="text-sm font-black text-slate-400 group-hover:text-white transition-colors">{label}</span>
    <div className="flex items-center gap-3">
      <span className="text-[10px] font-black text-slate-600 italic uppercase tracking-widest">{status}</span>
      <div className="w-1.5 h-1.5 bg-emerald-500 rounded-full"></div>
    </div>
  </div>
);
