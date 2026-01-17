
export interface NavigationItem {
  name: string;
  to: string;
}
export interface NavbarLinkProps {
  item: NavigationItem;
  onClick: () => void;
  className?: string;
}
