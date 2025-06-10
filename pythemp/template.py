import json
import time
import math

class AgentAPI:
    def __init__(self, agent_id):
        self.id = agent_id
        self.position = {"x": 0, "y": 0}
        self._running = True
        self.color = {"r": 0,"g": 0, "b":0}
        self.angle = 0;
        self.view_distance = 2000
        self.view_angle = 360
    def get_id(self):
        return self.id
    def command(self,command,**params):
        print(json.dumps({"command":command,"params":params,"agent_id":self.id}),flush=True)

    def move(self, dx, dy):
        self.position["x"] +=dx
        self.position["y"] +=dy
        self.command("move",dx=dx,dy=dy)

    def set_position(self, x, y):
        self.position["x"] =x
        self.position["y"] =y
        self.command("set_position",x=x,y=y)

    def get_position(self):
        return self.position

    def set_color(self,r,g,b):
        self.color["r"] = r
        self.color["g"] = g
        self.color["b"] = b
        self.command("set_color",r=r,g=g,b=b)

    def get_color(self):
        return self.color

    def rotate(self, angle):
        self.angle += angle
        self.command("rotate",angle=angle)
    def get_rotation(self):
        return self.angle
    def set_rotation(self,angle):
        self.angle = angle
        self.command("set_angle",angle=angle)

    def send_message(self,reciver,message):
        self.command("message",reciver = reciver,message = message)

    def spawn(self,entity_type,entity_name,entity_pos):
        self.command("spawn",entity_type = entity_type, entity_name = entity_name,entity_pos = entity_pos)

    def get_angle(self):
        return self.angle

    def set_view_angle(self, angle):
        self.view_angle = angle

    def get_view_angle(self):
        return self.view_angle

    def set_view_distance(self,distance):
        self.view_distance = distance

    def get_view_distance(self):
        return self.view_distance

    def stop(self):
        self._running = False
        print(self.command("stop"), flush=True)

    def get_visible_objects(self, state):
        """
        Возвращает объекты и агентов в зоне видимости текущего агента,
        разделенные на две категории: agents и objects
        """
        if isinstance(state, str):
            state = json.loads(state)

        visible_agents = []
        visible_objects = []
        my_pos = self.position

        # Обрабатываем объекты
        for obj in state.get("objects", {}).values():
            obj_pos = obj.get("position", {})
            if not obj_pos:
                continue

            distance_info = self.calculate_relative_position(my_pos, obj_pos, self.angle)
            if self.is_visible(distance_info):
                visible_objects.append(self._create_entity_info(obj, obj_pos, distance_info, False))

        # Обрабатываем агентов (исключая текущего агента)
        for agent in state.get("agents", {}).values():
            if agent.get("id") == self.id:
                continue
            agent_pos = agent.get("position", {})
            if not agent_pos:
                continue

            distance_info = self.calculate_relative_position(my_pos, agent_pos, self.angle)
            if self.is_visible(distance_info):
                visible_agents.append(self._create_entity_info(agent, agent_pos, distance_info, True))

        # Сортируем оба списка по расстоянию
        visible_agents.sort(key=lambda x: x["distance"])
        visible_objects.sort(key=lambda x: x["distance"])

        return {
            "agents": visible_agents,
            "objects": visible_objects
        }

    def calculate_relative_position(self, pos1, pos2, angle1_deg=0):
        """
        Универсальная функция для расчета относительного положения между двумя точками
        Возвращает словарь с информацией о расстоянии и углах

        Args:
            pos1: dict {"x": x1, "y": y1} - позиция первого объекта
            pos2: dict {"x": x2, "y": y2} - позиция второго объекта
            angle1_deg: float - угол направления первого объекта в градусах (необязательный)

        Returns:
            dict: {
                "distance": float,  # расстояние между объектами
                "dx": float,        # разница по x (pos2.x - pos1.x)
                "dy": float,        # разница по y (pos2.y - pos1.y)
                "absolute_angle": float,  # абсолютный угол от pos1 к pos2 (0-360°)
                "relative_angle": float   # угол относительно направления pos1 (-180° до +180°)
            }
        """
        dx = pos2["x"] - pos1["x"]
        dy = pos2["y"] - pos1["y"]

        distance = math.hypot(dx, dy)
        absolute_angle = math.degrees(math.atan2(dy, dx)) % 360

        angle1_rad = math.radians(angle1_deg)
        relative_angle = (absolute_angle - angle1_deg + 180) % 360 - 180

        return {
            "distance": distance,
            "dx": dx,
            "dy": dy,
            "absolute_angle": absolute_angle,
            "relative_angle": relative_angle
        }

    def is_visible(self, distance_info):
        """
        Проверяет, находится ли объект в поле зрения
        """
        return (distance_info["distance"] <= self.view_distance and
                abs(distance_info["relative_angle"]) <= self.view_angle/2)

    def _create_entity_info(self, entity, entity_pos, distance_info, is_agent):
        """
        Создает информацию о сущности для возврата
        """
        return {
            "id": entity.get("id"),
            "type": "agent" if is_agent else "object",
            "name": entity.get("agent_name") if is_agent else entity.get("shapeName"),
            "distance": distance_info["distance"],
            "angle": distance_info["absolute_angle"],
            "relative_angle": distance_info["relative_angle"],
            "position": entity_pos,
            "shape": entity.get("shape", {}),
            "shapeName": entity.get("shapeName","placeholder"),
            "color": entity.get("color",{})
        }
# Глобальный объект API
agent = AgentAPI(AGENT_ID_REPLACE)

{USER_CODE}

def main_loop():
    """Основной цикл обработки состояний"""
    while agent._running:
        try:
            state = json.loads(input())
            if not state:
                time.sleep(1)
                continue
            agents = state.get("agents", {})
            agent_data = agents.get(str(agent.id))
            if agent_data:
                pos = agent_data.get("position", {})
                if pos:
                    agent.position["x"] = pos.get("x", agent.position["x"])
                    agent.position["y"] = pos.get("y", agent.position["y"])
            process_tick(state)
        except Exception as e:
            print(json.dumps({"error": str(e)}))
                

# Автоматический запуск
if __name__ == "__main__":
    try:
        main_loop()
    finally:
        pass


