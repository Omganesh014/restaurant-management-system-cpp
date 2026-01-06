
import { Customer, MenuItem, Order, OrderStatus, Bill, BackendResponse, OrderItem, SystemLog } from '../types';

let customers: Customer[] = [
  { id: 'C001', name: 'John Doe', phone: '555-0101' },
  { id: 'C002', name: 'Jane Smith', phone: '555-0102' }
];

let menu: MenuItem[] = [
  { id: 'M001', name: 'Classic Burger', price: 12.99, enabled: true },
  { id: 'M002', name: 'Margherita Pizza', price: 15.50, enabled: true },
  { id: 'M003', name: 'Garden Salad', price: 8.95, enabled: true },
  { id: 'M004', name: 'Iced Tea', price: 2.50, enabled: false }
];

let orders: Order[] = [];
let bills: Bill[] = [];
let logListeners: ((log: SystemLog) => void)[] = [];

const addLog = (level: SystemLog['level'], message: string) => {
  const log: SystemLog = {
    timestamp: new Date().toLocaleTimeString('en-GB', { hour12: false }),
    level,
    message
  };
  logListeners.forEach(l => l(log));
};

const delay = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

export const backendService = {
  subscribeToLogs(callback: (log: SystemLog) => void) {
    logListeners.push(callback);
    return () => { logListeners = logListeners.filter(l => l !== callback); };
  },

  async addCustomer(name: string, phone: string): Promise<BackendResponse<Customer>> {
    addLog('IO', "Attempting write to customers.csv...");
    await delay(300);
    if (!name || !phone) {
      addLog('ERROR', "Validation failed: Customer name/phone required.");
      return { success: false, message: "Invalid customer data" };
    }
    const newCustomer: Customer = {
      id: `C${(customers.length + 1).toString().padStart(3, '0')}`,
      name,
      phone
    };
    customers.push(newCustomer);
    addLog('INFO', `Registered Guest ${newCustomer.id} to disk.`);
    return { success: true, message: "Customer added", data: newCustomer };
  },

  async getAllCustomers(): Promise<BackendResponse<Customer[]>> {
    addLog('IO', "Parsing customers.csv...");
    await delay(200);
    return { success: true, message: "Success", data: [...customers] };
  },

  async searchCustomerById(id: string): Promise<BackendResponse<Customer>> {
    await delay(150);
    const customer = customers.find(c => c.id === id);
    if (!customer) return { success: false, message: "Not found" };
    return { success: true, message: "Found", data: customer };
  },

  async getMenu(): Promise<BackendResponse<MenuItem[]>> {
    addLog('IO', "Reading menu_manifest.txt...");
    await delay(200);
    return { success: true, message: "Success", data: [...menu] };
  },

  async addMenuItem(name: string, price: number): Promise<BackendResponse<MenuItem>> {
    await delay(300);
    const newItem: MenuItem = {
      id: `M${(menu.length + 1).toString().padStart(3, '0')}`,
      name,
      price,
      enabled: true
    };
    menu.push(newItem);
    addLog('INFO', `Menu catalog updated: Added ${newItem.id}`);
    return { success: true, message: "Item added", data: newItem };
  },

  async toggleMenuItem(id: string, enabled: boolean): Promise<BackendResponse<MenuItem>> {
    await delay(200);
    const item = menu.find(m => m.id === id);
    if (!item) return { success: false, message: "Not found" };
    item.enabled = enabled;
    addLog('WARN', `Visibility bit for ${id} set to ${enabled}`);
    return { success: true, message: "Updated", data: item };
  },

  async placeOrder(customerId: string, items: OrderItem[]): Promise<BackendResponse<Order>> {
    addLog('INFO', `Order sequence initiated for GUEST: ${customerId}`);
    await delay(400);
    let total = 0;
    const enrichedItems = items.map(item => {
      const menuItem = menu.find(m => m.id === item.itemId);
      const price = menuItem ? menuItem.price : 0;
      total += price * item.quantity;
      return { ...item, name: menuItem?.name, price };
    });

    const newOrder: Order = {
      id: `ORD${(orders.length + 101).toString()}`,
      customerId,
      items: enrichedItems,
      status: OrderStatus.CREATED,
      total,
      timestamp: Date.now()
    };
    orders.push(newOrder);
    addLog('IO', `Synchronized ${newOrder.id} to ORDER_STREAMS.csv`);
    return { success: true, message: "Success", data: newOrder };
  },

  async getActiveOrders(): Promise<BackendResponse<Order[]>> {
    await delay(250);
    const active = orders.filter(o => o.status !== OrderStatus.SERVED && o.status !== OrderStatus.CANCELLED);
    return { success: true, message: "Success", data: active };
  },

  async updateOrderState(orderId: string, nextStatus: OrderStatus): Promise<BackendResponse<Order>> {
    addLog('INFO', `FSM Transition Attempt: ${orderId} -> ${nextStatus}`);
    await delay(300);
    const orderIndex = orders.findIndex(o => o.id === orderId);
    if (orderIndex === -1) return { success: false, message: "Order not found" };
    
    orders[orderIndex].status = nextStatus;
    addLog('INFO', `FSM State ${orderId} finalized as ${nextStatus}`);
    return { success: true, message: "Updated", data: orders[orderIndex] };
  },

  async getKitchenOrders(): Promise<BackendResponse<Order[]>> {
    await delay(200);
    const kitchenStatuses = [OrderStatus.CONFIRMED, OrderStatus.PREPARING, OrderStatus.READY];
    return { success: true, message: "Success", data: orders.filter(o => kitchenStatuses.includes(o.status)) };
  },

  async generateBill(orderId: string): Promise<BackendResponse<Bill>> {
    addLog('IO', `Computing settlement for ${orderId}...`);
    await delay(300);
    const order = orders.find(o => o.id === orderId);
    if (!order) return { success: false, message: "Order not found" };
    
    const newBill: Bill = {
      id: `BILL${(bills.length + 1001).toString()}`,
      orderId: order.id,
      total: order.total,
      status: 'UNPAID'
    };
    bills.push(newBill);
    return { success: true, message: "Success", data: newBill };
  },

  async processPayment(billId: string): Promise<BackendResponse<Bill>> {
    addLog('INFO', `Verifying transaction for ${billId}`);
    await delay(500);
    const bill = bills.find(b => b.id === billId);
    if (bill) bill.status = 'PAID';
    addLog('INFO', `Settlement ${billId} locked.`);
    return { success: true, message: "Payment verified", data: bill };
  },

  async getAllBills(): Promise<BackendResponse<Bill[]>> {
    await delay(200);
    return { success: true, message: "Success", data: [...bills] };
  }
};
