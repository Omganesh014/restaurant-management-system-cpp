
import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { MenuItem } from '../types';
import { backendService } from '../services/backendService';

export const MenuModule: React.FC = () => {
  const [menu, setMenu] = useState<MenuItem[]>([]);
  const [name, setName] = useState('');
  const [price, setPrice] = useState('');
  const [loading, setLoading] = useState(false);

  const fetchMenu = async () => {
    setLoading(true);
    const res = await backendService.getMenu();
    if (res.success && res.data) setMenu(res.data);
    setLoading(false);
  };

  useEffect(() => { fetchMenu(); }, []);

  const handleAdd = async (e: React.FormEvent) => {
    e.preventDefault();
    const res = await backendService.addMenuItem(name, parseFloat(price));
    if (res.success) {
      setName(''); setPrice(''); fetchMenu();
    }
  };

  const handleToggle = async (id: string, current: boolean) => {
    const res = await backendService.toggleMenuItem(id, !current);
    if (res.success) fetchMenu();
  };

  return (
    <div className="space-y-12">
      <div className="glass-dark p-12 rounded-[3.5rem] shadow-2xl border border-white/5">
        <h2 className="text-3xl font-black text-white italic uppercase tracking-tighter mb-10 leading-none">Inventory_Sync</h2>
        <form onSubmit={handleAdd} className="flex flex-col md:flex-row gap-6">
          <input className="flex-1 bg-white/5 border border-white/10 p-6 rounded-[1.5rem] text-sm font-black text-white outline-none focus:ring-4 focus:ring-cyan-500/20 transition-all placeholder:text-slate-600" placeholder="ITEM NAME" value={name} onChange={(e) => setName(e.target.value)} required />
          <div className="md:w-48 relative">
            <span className="absolute left-6 top-1/2 -translate-y-1/2 text-slate-500 font-black text-xl leading-none">$</span>
            <input className="w-full bg-white/5 border border-white/10 p-6 pl-12 rounded-[1.5rem] text-xl font-black text-white outline-none focus:ring-4 focus:ring-cyan-500/20 transition-all mono" type="number" step="0.01" value={price} onChange={(e) => setPrice(e.target.value)} required />
          </div>
          <button type="submit" className="bg-cyan-600 hover:bg-cyan-500 text-white px-12 py-6 rounded-[1.5rem] font-black text-xs uppercase tracking-[0.3em] active:scale-95 shadow-2xl shadow-cyan-600/20 transition-all">Add to Store</button>
        </form>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 xl:grid-cols-3 gap-8">
        {menu.map((item, i) => (
          <motion.div 
            initial={{ opacity: 0, scale: 0.9 }} 
            animate={{ opacity: 1, scale: 1 }} 
            transition={{ delay: i * 0.05 }} 
            key={item.id} 
            className={`p-10 glass-dark rounded-[3rem] shadow-xl flex flex-col justify-between group h-64 border-2 transition-all duration-500 ${!item.enabled ? 'opacity-60 border-rose-500/20' : 'border-transparent hover:border-cyan-500/20'}`}
          >
            <div className="flex justify-between items-start">
              <div className="space-y-2">
                <span className="mono text-[11px] text-slate-500 font-black uppercase">{item.id}</span>
                <p className="text-2xl font-black text-white leading-tight uppercase italic group-hover:text-cyan-400 transition-colors">{item.name}</p>
              </div>
              <div className="relative">
                 {!item.enabled && <div className="absolute inset-0 bg-rose-500 rounded-full animate-ping opacity-25"></div>}
                 <div className={`w-3.5 h-3.5 rounded-full relative z-10 ${item.enabled ? 'bg-emerald-500 shadow-[0_0_15px_#10b981]' : 'bg-rose-500 shadow-[0_0_15px_#f43f5e]'}`}></div>
              </div>
            </div>
            <div className="flex justify-between items-end">
              <span className="text-4xl font-black text-white mono tracking-tighter leading-none">${item.price.toFixed(2)}</span>
              <button onClick={() => handleToggle(item.id, item.enabled)} className={`px-8 py-3 rounded-2xl text-[10px] font-black uppercase tracking-widest transition-all ${item.enabled ? 'bg-white/5 text-slate-400 hover:bg-rose-500 hover:text-white' : 'bg-cyan-600 text-white hover:bg-cyan-500'}`}>
                {item.enabled ? 'Disable' : 'Enable'}
              </button>
            </div>
          </motion.div>
        ))}
      </div>
    </div>
  );
};
