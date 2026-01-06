
import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { Customer } from '../types';
import { backendService } from '../services/backendService';

export const CustomerModule: React.FC = () => {
  const [customers, setCustomers] = useState<Customer[]>([]);
  const [name, setName] = useState('');
  const [phone, setPhone] = useState('');
  const [loading, setLoading] = useState(false);

  const fetchCustomers = async () => {
    setLoading(true);
    const res = await backendService.getAllCustomers();
    if (res.success && res.data) setCustomers(res.data);
    setLoading(false);
  };

  useEffect(() => { fetchCustomers(); }, []);

  const handleAdd = async (e: React.FormEvent) => {
    e.preventDefault();
    const res = await backendService.addCustomer(name, phone);
    if (res.success) {
      setName(''); setPhone(''); fetchCustomers();
    }
  };

  return (
    <div className="space-y-12">
      <div className="glass-dark p-12 rounded-[3.5rem] shadow-2xl">
        <h2 className="text-3xl font-black text-white italic uppercase tracking-tighter mb-10 leading-none">Guest_Registry</h2>
        <form onSubmit={handleAdd} className="grid grid-cols-1 md:grid-cols-3 gap-6">
          <input className="bg-white/5 border border-white/10 p-6 rounded-[1.5rem] text-sm font-black text-white focus:ring-4 focus:ring-indigo-500/20 outline-none transition-all placeholder:text-slate-600" placeholder="FULL NAME" value={name} onChange={(e) => setName(e.target.value)} required />
          <input className="bg-white/5 border border-white/10 p-6 rounded-[1.5rem] text-sm font-black text-white focus:ring-4 focus:ring-indigo-500/20 outline-none transition-all placeholder:text-slate-600" placeholder="CONTACT PHONE" value={phone} onChange={(e) => setPhone(e.target.value)} required />
          <button type="submit" className="bg-indigo-600 hover:bg-indigo-500 text-white p-6 rounded-[1.5rem] font-black text-xs uppercase tracking-[0.3em] transition-all active:scale-95 shadow-2xl shadow-indigo-600/20">Write to Disk</button>
        </form>
      </div>

      <div className="glass-dark rounded-[4rem] overflow-hidden shadow-2xl">
        <div className="p-10 border-b border-white/5 flex justify-between items-center bg-white/[0.02]">
          <h2 className="text-xl font-black text-white uppercase tracking-widest leading-none italic">Verified_Directory</h2>
          <span className="mono text-[10px] text-slate-500 font-black">{customers.length} RECORDS</span>
        </div>
        <div className="divide-y divide-white/5">
          {customers.map((c, i) => (
            <motion.div initial={{ opacity: 0 }} animate={{ opacity: 1 }} transition={{ delay: i * 0.05 }} key={c.id} className="p-10 flex flex-col md:flex-row md:items-center justify-between hover:bg-white/[0.02] transition-colors group">
              <div className="flex items-center gap-8">
                <span className="mono text-sm font-black text-indigo-500 bg-indigo-500/10 px-4 py-2 rounded-xl group-hover:bg-indigo-500 group-hover:text-white transition-all">{c.id}</span>
                <div className="space-y-1">
                  <p className="text-2xl font-black text-white uppercase italic tracking-tighter leading-none">{c.name}</p>
                  <p className="text-[11px] font-black text-slate-500 uppercase tracking-widest">{c.phone}</p>
                </div>
              </div>
              <div className="mt-4 md:mt-0 text-[10px] font-black text-slate-700 uppercase tracking-[0.4em] italic group-hover:text-indigo-400 transition-colors">IO_Persisted_Node</div>
            </motion.div>
          ))}
          {customers.length === 0 && <div className="p-20 text-center opacity-10 font-black uppercase text-2xl tracking-[0.5em]">Ledger_Empty</div>}
        </div>
      </div>
    </div>
  );
};
