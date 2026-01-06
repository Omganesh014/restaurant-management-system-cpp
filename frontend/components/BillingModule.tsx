
import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { Bill, Order, OrderStatus } from '../types';
import { backendService } from '../services/backendService';

export const BillingModule: React.FC = () => {
  const [bills, setBills] = useState<Bill[]>([]);
  const [readyOrders, setReadyOrders] = useState<Order[]>([]);
  const [activeReceipt, setActiveReceipt] = useState<Order | null>(null);

  const refreshData = async () => {
    const [bRes, oRes] = await Promise.all([
      backendService.getAllBills(),
      backendService.getActiveOrders()
    ]);
    if (bRes.success) setBills(bRes.data || []);
    if (oRes.success) {
      setReadyOrders((oRes.data || []).filter(o => o.status === OrderStatus.READY || o.status === OrderStatus.SERVED));
    }
  };

  useEffect(() => { refreshData(); }, []);

  const handleGenerate = async (orderId: string) => {
    const res = await backendService.generateBill(orderId);
    if (res.success) refreshData();
  };

  const handlePay = async (billId: string) => {
    const res = await backendService.processPayment(billId);
    if (res.success) refreshData();
  };

  return (
    <div className="grid grid-cols-1 xl:grid-cols-2 gap-16">
      <div className="space-y-10">
        <h2 className="text-3xl font-black text-white italic uppercase tracking-tighter leading-none">Pending_Invoices</h2>
        <div className="space-y-6">
          {readyOrders.filter(o => !bills.find(b => b.orderId === o.id)).map(order => (
            <motion.div key={order.id} className="p-10 glass-dark rounded-[3rem] flex justify-between items-center group shadow-2xl">
              <div className="space-y-3">
                <span className="mono text-[11px] text-slate-500 font-black">{order.id}</span>
                <p className="text-4xl font-black text-white mono leading-none">${order.total.toFixed(2)}</p>
                <button onClick={() => setActiveReceipt(order)} className="text-[10px] font-black text-indigo-400 uppercase tracking-widest flex items-center gap-2 hover:text-indigo-300 transition-colors">
                  <svg className="w-3.5 h-3.5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path d="M15 12a3 3 0 11-6 0 3 3 0 016 0z"/><path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2.5" d="M2.458 12C3.732 7.943 7.523 5 12 5c4.478 0 8.268 2.943 9.542 7-1.274 4.057-5.064 7-9.542 7-4.477 0-8.268-2.943-9.542-7z"/></svg>
                  View Digital Receipt
                </button>
              </div>
              <button onClick={() => handleGenerate(order.id)} className="px-10 py-5 bg-indigo-600 rounded-[1.5rem] text-[11px] font-black uppercase tracking-widest text-white hover:bg-indigo-500 active:scale-95 transition-all shadow-2xl shadow-indigo-600/20">Finalize Bill</button>
            </motion.div>
          ))}
          {readyOrders.filter(o => !bills.find(b => b.orderId === o.id)).length === 0 && (
            <div className="py-20 text-center glass-dark rounded-[3rem] opacity-20 font-black uppercase tracking-[0.4em] text-sm">No Open Invoices</div>
          )}
        </div>
      </div>

      <div className="space-y-10">
        <h2 className="text-3xl font-black text-white italic uppercase tracking-tighter leading-none">System_Ledger</h2>
        <div className="glass-dark rounded-[4rem] overflow-hidden flex flex-col shadow-2xl">
          <div className="p-10 border-b border-white/5 bg-white/[0.02]">
            <div className="grid grid-cols-4 text-[11px] font-black text-slate-500 uppercase tracking-widest">
              <span>Invoice_ID</span>
              <span>Amount</span>
              <span>Status</span>
              <span className="text-right">Sync</span>
            </div>
          </div>
          <div className="divide-y divide-white/5 overflow-y-auto scrollbar-hide">
            {bills.map(bill => (
              <div key={bill.id} className="p-10 grid grid-cols-4 items-center hover:bg-white/[0.02] transition-colors">
                <span className="mono text-xs font-bold text-slate-400">{bill.id}</span>
                <span className="mono text-lg font-black text-white leading-none">${bill.total.toFixed(2)}</span>
                <div>
                  <span className={`px-4 py-1.5 rounded-full text-[9px] font-black uppercase tracking-widest border ${bill.status === 'PAID' ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/20' : 'bg-rose-500/10 text-rose-400 border-rose-500/20'}`}>
                    {bill.status}
                  </span>
                </div>
                <div className="text-right">
                  {bill.status === 'UNPAID' ? (
                    <button onClick={() => handlePay(bill.id)} className="px-6 py-2.5 bg-emerald-600 rounded-xl text-[10px] font-black uppercase tracking-widest text-white hover:bg-emerald-500 transition-all active:scale-95">Settle</button>
                  ) : (
                    <span className="text-[10px] font-black text-slate-600 uppercase italic">Locked_Disk</span>
                  )}
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>

      <AnimatePresence>
        {activeReceipt && (
          <div className="fixed inset-0 z-[200] flex items-center justify-center p-10">
            <motion.div initial={{ opacity: 0 }} animate={{ opacity: 1 }} exit={{ opacity: 0 }} onClick={() => setActiveReceipt(null)} className="absolute inset-0 bg-black/90 backdrop-blur-2xl" />
            <motion.div initial={{ scale: 0.9, y: 30 }} animate={{ scale: 1, y: 0 }} exit={{ scale: 0.9, y: 30 }} className="relative w-full max-w-sm receipt-paper rounded-t-lg flex flex-col">
              <div className="p-12 text-center space-y-6 receipt-font text-zinc-800">
                <h3 className="text-5xl font-black uppercase italic leading-none mb-2">RMS TERMINAL</h3>
                <div className="text-xs border-y border-dashed border-zinc-300 py-3 uppercase tracking-tighter">
                  Terminal: POS_01 / User: SYSTEM<br/>
                  {new Date().toLocaleString()}
                </div>
                <div className="space-y-4 py-8">
                  {activeReceipt.items.map((item, idx) => (
                    <div key={idx} className="flex justify-between items-start text-xl leading-none">
                      <div className="text-left font-bold">{item.quantity}x {item.name}</div>
                      <span className="font-bold shrink-0 ml-4">${((item.price || 0) * item.quantity).toFixed(2)}</span>
                    </div>
                  ))}
                </div>
                <div className="border-t-4 border-double border-zinc-200 pt-8 space-y-2">
                  <div className="flex justify-between text-2xl"><span>SUBTOTAL</span><span>${activeReceipt.total.toFixed(2)}</span></div>
                  <div className="flex justify-between text-4xl font-black pt-4"><span>TOTAL</span><span>${activeReceipt.total.toFixed(2)}</span></div>
                </div>
                <div className="pt-12 space-y-2">
                  <p className="text-xl font-bold">STATION LOGS FINALIZED</p>
                  <p className="text-xs opacity-60 italic leading-none">C++ Core Session Hash: {activeReceipt.id}</p>
                </div>
                <div className="pt-8 flex flex-col items-center gap-2">
                  <div className="w-full h-1 bg-zinc-200"></div>
                  <div className="w-3/4 h-1 bg-zinc-200"></div>
                </div>
              </div>
              <button onClick={() => setActiveReceipt(null)} className="bg-zinc-900 text-white py-8 font-black text-sm uppercase tracking-[0.5em] hover:bg-zinc-800 transition-colors rounded-b-lg">Close Terminal View</button>
            </motion.div>
          </div>
        )}
      </AnimatePresence>
    </div>
  );
};
