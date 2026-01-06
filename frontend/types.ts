
export enum OrderStatus {
  CREATED = 'CREATED',
  CONFIRMED = 'CONFIRMED',
  PREPARING = 'PREPARING',
  READY = 'READY',
  SERVED = 'SERVED',
  CANCELLED = 'CANCELLED'
}

export interface Customer {
  id: string;
  name: string;
  phone: string;
}

export interface MenuItem {
  id: string;
  name: string;
  price: number;
  enabled: boolean;
}

export interface OrderItem {
  itemId: string;
  quantity: number;
  name?: string; 
  price?: number; 
}

export interface Order {
  id: string;
  customerId: string;
  items: OrderItem[];
  status: OrderStatus;
  total: number;
  timestamp: number;
}

export interface Bill {
  id: string;
  orderId: string;
  total: number;
  status: 'UNPAID' | 'PAID';
}

export interface SystemLog {
  timestamp: string;
  level: 'INFO' | 'WARN' | 'ERROR' | 'IO';
  message: string;
}

export interface BackendResponse<T> {
  success: boolean;
  message: string;
  data?: T;
}

export type ViewType = 'DASHBOARD' | 'CUSTOMERS' | 'MENU' | 'ORDERS' | 'KITCHEN' | 'BILLING' | 'HEALTH';
