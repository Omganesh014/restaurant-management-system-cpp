
import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { Order, MenuItem, OrderStatus, OrderItem, Customer } from '../types';
import { backendService } from '../services/backendService';

const statusSteps = [
  OrderStatus.CREATED,
  OrderStatus.CONFIRMED,
  OrderStatus.PREPARING,
  OrderStatus.READY,
  OrderStatus.SERVED
];

export const OrderModule: React.FC = () => {
  const [orders, setOrders] = useState<Order[]>([]);
  const [menu, setMenu] = useState<MenuItem[]>([]);
  const [customers, setCustomers] = useState<Customer[]>([]);
  const [selectedCustomerId, setSelectedCustomerId] = useState('');
  const [cart, setCart] = useState<OrderItem[]>([]);
  const [loading, setLoading] = useState(false);

  const init = async () => {
    setLoading(true);
    const [mRes, oRes, cRes] = await Promise.all([
      backendService.getMenu(),
      backendService.getActiveOrders(),
      backendService.getAllCustomers()
    ]);
    if (mRes.success) setMenu(mRes.data || []);
    if (oRes.success) setOrders(oRes.data || []);
    if (cRes.success) setCustomers(cRes.data || []);
    setLoading(false);
  };

  useEffect(() => { init(); }, []);

  const addToCart = (itemId: string) => {
    const existing = cart.find(i => i.itemId === itemId);
    if (existing) {
      setCart(cart.map(i => i.itemId === itemId ? { ...i, quantity: i.quantity + 1 } : i));
    } else {
      setCart([...cart, { itemId, quantity: 1 }]);
    }
  };

  const handlePlaceOrder = async () => {
    if (!selectedCustomerId || cart.length === 0) return;
    const res = await backendService.placeOrder(selectedCustomerId, cart);
    if (res.success) {
      setCart([]);
      setSelectedCustomerId('');
      init();
    }
  };

  const cartTotal = cart.reduce((acc, curr) => {
    const item = menu.find(m => m.id === curr.itemId);
    return acc + (item?.price || 0) * curr.quantity;
  }, 0);

  return (
    <div className="space-y-16">
      <div className="grid grid-cols-1 lg:grid-cols-12 gap-12 h-full items-start">
        {/* GRID CATALOG */}
        <div className="lg:col-span-8 space-y-10">
          <div className="flex flex-col md:flex-row gap-6 items-center justify-between">
            <h2 className="text-3xl font-black text-white italic uppercase tracking-tighter leading-none">Order_Entry</h2>
            <select 
              className="bg-white/5 border border-white/10 p-5 rounded-[1.5rem] text-sm font-black text-white outline-none focus:ring-4 focus:ring-indigo-500/20 w-full md:w-80 transition-all"
              value={selectedCustomerId}
              onChange={(e) => setSelectedCustomerId(e.target.value)}
            >
              <option value="" className="bg-slate-900">-- SELECT GUEST --</option>
              {customers.map(c => <option key={c.id} value={c.id} className="bg-slate-900">{c.name} ({c.id})</option>)}
            </select>
          </div>

          <div className="grid grid-cols-2 sm:grid-cols-3 xl:grid-cols-4 gap-6">
            {menu.filter(m => m.enabled).map(item => (
              <button key={item.id} onClick={() => addToCart(item.id)} className="p-10 glass-dark rounded-[3rem] text-left hover:bg-indigo-600 transition-all group flex flex-col justify-between h-52 active:scale-95 shadow-xl border border-white/5">
                <span className="text-base font-black text-white uppercase leading-tight group-hover:text-white">{item.name}</span>
                <div className="flex flex-col">
                  <span className="mono text-[10px] text-slate-500 font-bold group-hover:text-indigo-200">{item.id}</span>
                  <span className="mono text-2xl font-black text-indigo-400 group-hover:text-white mt-1">${item.price.toFixed(2)}</span>
                </div>
              </button>
            ))}
          </div>
        </div>

        {/* POS SIDE CART */}
        <div className="lg:col-span-4 flex flex-col h-full sticky top-32">
          <div className="glass-dark p-12 rounded-[3.5rem] flex-1 flex flex-col shadow-2xl border border-white/5">
            <div className="flex justify-between items-center mb-10">
              <h2 className="text-xl font-black text-white tracking-tighter uppercase italic">Registry</h2>
              <div className={`px-4 py-1.5 rounded-full text-[10px] font-black uppercase ${cart.length > 0 ? 'bg-indigo-500 text-white' : 'bg-white/5 text-slate-500'}`}>
                {cart.length} Nodes
              </div>
            </div>

            <div className="flex-1 overflow-y-auto scrollbar-hide space-y-6 pr-2 mb-8 min-h-[200px]">
              <AnimatePresence mode="popLayout">
                {cart.map(ci => {
                  const item = menu.find(m => m.id === ci.itemId);
                  return (
                    <motion.div initial={{ opacity: 0, x: 20 }} animate={{ opacity: 1, x: 0 }} exit={{ opacity: 0, scale: 0.9 }} key={ci.itemId} className="flex justify-between items-start group">
                      <div className="space-y-1">
                        <p className="text-sm font-black text-white uppercase leading-none group-hover:text-indigo-400 transition-colors">{item?.name}</p>
                        <p className="text-[10px] font-black text-slate-500 uppercase tracking-widest">Quantity: {ci.quantity}</p>
                      </div>
                      <span className="mono text-sm font-black text-indigo-400">${((item?.price || 0) * ci.quantity).toFixed(2)}</span>
                    </motion.div>
                  );
                })}
              </AnimatePresence>
              {cart.length === 0 && (
                <div className="h-full flex flex-col items-center justify-center text-slate-600 font-black uppercase tracking-widest text-[10px] italic py-20">Awaiting input...</div>
              )}
            </div>

            <div className="pt-8 border-t border-white/5 space-y-6">
              <div className="flex justify-between items-end">
                <span className="text-[10px] font-black text-slate-500 uppercase tracking-[0.3em]">Commit_Total</span>
                <span className="text-5xl font-black text-white mono leading-none tracking-tighter">${cartTotal.toFixed(2)}</span>
              </div>
              <button 
                disabled={cart.length === 0 || !selectedCustomerId} 
                onClick={handlePlaceOrder}
                className={`w-full py-7 rounded-[2rem] font-black text-xs uppercase tracking-[0.4em] transition-all active:scale-95 shadow-2xl ${
                  cart.length > 0 && selectedCustomerId ? 'bg-gradient-to-br from-indigo-500 to-indigo-700 text-white shadow-indigo-600/30 hover:shadow-indigo-600/50' : 'bg-white/5 text-slate-700'
                }`}
              >
                Sync Transaction
              </button>
            </div>
          </div>
        </div>
      </div>

      {/* FSM STATE TRACKER (ACTIVE ORDERS) */}
      <div className="space-y-10">
         <h2 className="text-3xl font-black text-white italic uppercase tracking-tighter leading-none">Active_Flows</h2>
         <div className="grid grid-cols-1 gap-8">
            {orders.map(order => (
              <motion.div key={order.id} className="glass-dark p-10 rounded-[3.5rem] border border-white/5 shadow-xl">
                <div className="flex flex-col md:flex-row justify-between items-start md:items-center gap-6 mb-12">
                  <div className="space-y-2">
                    <span className="mono text-[11px] font-black text-slate-500 uppercase tracking-widest">Order_Node: {order.id}</span>
                    <p className="text-3xl font-black text-white italic uppercase leading-none">Guest_{order.customerId}</p>
                  </div>
                  <div className="flex gap-12">
                     <Telemetry label="Node_Total" value={`$${order.total.toFixed(2)}`} />
                     <Telemetry label="Status" value={order.status} />
                  </div>
                </div>

                {/* FSM Stepper */}
                <div className="relative pt-10 px-4">
                  <div className="absolute top-1/2 left-0 w-full h-1 bg-white/5 -translate-y-1/2 z-0"></div>
                  <div className="flex justify-between relative z-10">
                    {statusSteps.map((s, i) => {
                      const isActive = order.status === s;
                      const isPast = statusSteps.indexOf(order.status) >= i;
                      return (
                        <div key={s} className="flex flex-col items-center gap-4">
                          <div className={`w-10 h-10 rounded-full flex items-center justify-center transition-all duration-700 ${
                            isActive ? 'bg-indigo-500 shadow-[0_0_25px_#6366f1] scale-125' : 
                            isPast ? 'bg-indigo-500/40 text-white' : 'bg-slate-800 text-slate-600'
                          }`}>
                            {isPast && !isActive ? (
                              <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path strokeLinecap="round" strokeLinejoin="round" strokeWidth="4" d="M5 13l4 4L19 7" /></svg>
                            ) : (
                              <span className="mono font-black text-xs">{i + 1}</span>
                            )}
                          </div>
                          <span className={`text-[9px] font-black uppercase tracking-widest transition-colors ${isActive ? 'text-indigo-400' : isPast ? 'text-slate-400' : 'text-slate-700'}`}>
                            {s}
                          </span>
                        </div>
                      );
                    })}
                  </div>
                </div>
              </motion.div>
            ))}
            {orders.length === 0 && (
              <div className="py-24 glass-dark rounded-[4rem] flex items-center justify-center text-slate-700 font-black uppercase tracking-[0.5em] text-sm opacity-20 italic">No active state transitions</div>
            )}
         </div>
      </div>
    </div>
  );
};

const Telemetry = ({ label, value }: { label: string, value: string }) => (
  <div className="text-right">
    <p className="text-[10px] text-slate-500 font-black uppercase tracking-widest mb-1">{label}</p>
    <p className="text-xl font-black text-white mono leading-none">{value}</p>
  </div>
);
