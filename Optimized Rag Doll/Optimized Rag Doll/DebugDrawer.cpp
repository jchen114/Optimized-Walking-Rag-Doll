#include "stdafx.h"
#include "DebugDrawer.h"
#include "BulletOpenGLApplication.h"

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
	btVector3 a = from;
	btVector3 b = to;

	// draws a simple line of pixels between points.

	// use the GL_LINES primitive to draw lines

	glBegin(GL_LINES);
	glColor3f(color.getX(), color.getY(), color.getZ());
	glVertex3f(a.getX(), a.getY(), a.getZ());
	glVertex3f(b.getX(), b.getY(), b.getZ());
	glEnd();
	
	}
	
	void DebugDrawer::drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
		{
		// draws a line between two contact points
		btVector3 const startPoint = pointOnB;
		btVector3 const endPoint = pointOnB + normalOnB * distance;
		drawLine(startPoint, endPoint, color);
		
	}
	
	void DebugDrawer::ToggleDebugFlag(int flag) {
		// checks if a flag is set and enables/
		// disables it
		if (m_debugMode & flag)
			// flag is enabled, so disable it
			m_debugMode = m_debugMode & (~flag);
		else
			// flag is disabled, so enable it
			m_debugMode |= flag;
		
	}
