
import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { Order, OrderStatus } from '../types';
import { backendService } from '../services/backendService';

export const KitchenModule: React.FC = () => {
  const [orders, setOrders] = useState<Order[]>([]);
  const [loading, setLoading] = useState(false);

  const fetchQueue = async () => {
    setLoading(true);
    const res = await backendService.getKitchenOrders();
    if (res.success && res.data) setOrders(res.data);
    setLoading(false);
  };

  useEffect(() => {
    fetchQueue();
    const interval = setInterval(fetchQueue, 5000);
    return () => clearInterval(interval);
  }, []);

  const handleAction = async (id: string, next: OrderStatus) => {
    const res = await backendService.updateOrderState(id, next);
    if (res.success) fetchQueue();
  };

  return (
    <div className="space-y-12">
      <div className="flex flex-col md:flex-row justify-between items-center gap-6">
        <div>
          <h2 className="text-4xl font-black text-white tracking-tighter uppercase italic leading-none">Fire_Deck_V1</h2>
          <p className="text-amber-500 text-[10px] font-black tracking-[0.5em] uppercase mt-4">Kitchen Production Hub</p>
        </div>
        <div className="flex items-center gap-6 px-8 py-4 glass-dark rounded-3xl">
          <div className="w-2.5 h-2.5 bg-rose-500 rounded-full animate-pulse"></div>
          <span className="text-[11px] font-black text-slate-300 uppercase tracking-widest">{orders.length} Active Tickets</span>
        </div>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 xl:grid-cols-3 2xl:grid-cols-4 gap-8">
        <AnimatePresence mode="popLayout">
          {orders.map(order => (
            <motion.div 
              key={order.id} 
              layout 
              initial={{ opacity: 0, y: 40 }} 
              animate={{ opacity: 1, y: 0 }} 
              exit={{ opacity: 0, scale: 0.95 }}
              className={`p-10 rounded-[3rem] border-2 transition-all duration-700 flex flex-col justify-between min-h-[400px] ${
                order.status === OrderStatus.PREPARING 
                  ? 'bg-amber-500/10 border-amber-500 shadow-[0_0_60px_rgba(245,158,11,0.15)]' 
                  : 'glass-dark border-white/5'
              }`}
            >
              <div className="space-y-8">
                <div className="flex justify-between items-center">
                  <span className="mono text-[11px] font-black text-slate-500 uppercase">{order.id}</span>
                  <div className={`px-4 py-1.5 rounded-full text-[9px] font-black uppercase tracking-widest ${
                    order.status === OrderStatus.PREPARING ? 'bg-amber-500 text-black' : 'bg-white/10 text-white'
                  }`}>
                    {order.status}
                  </div>
                </div>
                
                <div className="space-y-6">
                  {order.items.map((item, idx) => (
                    <div key={idx} className="flex gap-6 items-start group">
                      <div className="w-12 h-12 glass-dark rounded-2xl flex items-center justify-center font-black text-xl text-white shrink-0 shadow-lg group-hover:bg-amber-500 group-hover:text-black transition-colors">
                        {item.quantity}
                      </div>
                      <div className="pt-1">
                        <p className="text-2xl font-black text-white leading-tight uppercase italic">{item.name}</p>
                        <div className={`h-1 w-8 mt-2 transition-all ${order.status === OrderStatus.PREPARING ? 'bg-amber-500' : 'bg-white/10'}`}></div>
                      </div>
                    </div>
                  ))}
                </div>
              </div>

              <div className="pt-12">
                {order.status === OrderStatus.CONFIRMED && (
                  <button onClick={() => handleAction(order.id, OrderStatus.PREPARING)} className="w-full py-6 bg-white text-black rounded-[2rem] font-black text-xs uppercase tracking-[0.3em] hover:bg-amber-500 transition-all active:scale-95 shadow-2xl">Start Production</button>
                )}
                {order.status === OrderStatus.PREPARING && (
                  <button onClick={() => handleAction(order.id, OrderStatus.READY)} className="w-full py-6 bg-amber-500 text-black rounded-[2rem] font-black text-xs uppercase tracking-[0.3em] hover:bg-amber-400 transition-all active:scale-95 shadow-xl shadow-amber-500/20">Mark Ready</button>
                )}
                {order.status === OrderStatus.READY && (
                  <div className="text-center py-6 text-[11px] font-black text-emerald-500 uppercase tracking-widest animate-pulse">Waiting for Server</div>
                )}
              </div>
            </motion.div>
          ))}
        </AnimatePresence>
        {orders.length === 0 && (
          <div className="col-span-full py-32 glass-dark rounded-[4rem] flex flex-col items-center justify-center space-y-4 opacity-30">
            <span className="text-6xl">❄️</span>
            <p className="font-black text-sm uppercase tracking-[0.5em]">Prep queue is clear</p>
          </div>
        )}
      </div>
    </div>
  );
};
